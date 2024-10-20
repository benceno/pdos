// Console input and output.
// Input is from the keyboard or serial port.
// Output is written to the screen and serial port.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "traps.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "history.h"


#define KEY_Down        0xE2
#define KEY_UP          0xE3
#define KEY_LEFT  0xE4
#define KEY_RIGHT 0xE5

#define COPY_BUF_SIZE 128  // Define a reasonable size for the buffer

char copy_buffer[COPY_BUF_SIZE];
int copy_mode = 0;  // 1 when in copy mode, 0 otherwise
int copylen = 0; // Tracks the number of copied characters

int back_counter;

static void consputc(int);

static int panicked = 0;

static struct {
  struct spinlock lock;
  int locking;
} cons;

static void
printint(int xx, int base, int sign)
{
  static char digits[] = "0123456789abcdef";
  char buf[16];
  int i;
  uint x;

  if(sign && (sign = xx < 0))
    x = -xx;
  else
    x = xx;

  i = 0;
  do{
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);

  if(sign)
    buf[i++] = '-';

  while(--i >= 0)
    consputc(buf[i]);
}
//PAGEBREAK: 50

// Print to the console. only understands %d, %x, %p, %s.
void
cprintf(char *fmt, ...)
{
  int i, c, locking;
  uint *argp;
  char *s;

  locking = cons.locking;
  if(locking)
    acquire(&cons.lock);

  if (fmt == 0)
    panic("null fmt");

  argp = (uint*)(void*)(&fmt + 1);
  for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
    if(c != '%'){
      consputc(c);
      continue;
    }
    c = fmt[++i] & 0xff;
    if(c == 0)
      break;
    switch(c){
    case 'd':
      printint(*argp++, 10, 1);
      break;
    case 'x':
    case 'p':
      printint(*argp++, 16, 0);
      break;
    case 's':
      if((s = (char*)*argp++) == 0)
        s = "(null)";
      for(; *s; s++)
        consputc(*s);
      break;
    case '%':
      consputc('%');
      break;
    default:
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c);
      break;
    }
  }

  if(locking)
    release(&cons.lock);
}

void
panic(char *s)
{
  int i;
  uint pcs[10];

  cli();
  cons.locking = 0;
  // use lapiccpunum so that we can call panic from mycpu()
  cprintf("lapicid %d: panic: ", lapicid());
  cprintf(s);
  cprintf("\n");
  getcallerpcs(&s, pcs);
  for(i=0; i<10; i++)
    cprintf(" %p", pcs[i]);
  panicked = 1; // freeze other CPU
  for(;;)
    ;
}

//PAGEBREAK: 50
#define BACKSPACE 0x100
#define CRTPORT 0x3d4
static ushort *crt = (ushort*)P2V(0xb8000);  // CGA memory

static void
cgaputc(int c)
{
  int pos;

  // Cursor position: col + 80*row.
  outb(CRTPORT, 14);
  pos = inb(CRTPORT+1) << 8;
  outb(CRTPORT, 15);
  pos |= inb(CRTPORT+1);

  if(c == '\n')
    pos += 80 - pos%80;
  else if(c == KEY_RIGHT){
    if (back_counter < 0){
    pos++;
    back_counter++;
    outb(CRTPORT+1, pos);
    }
    return;
  }
  else if(c == KEY_LEFT){
    if(pos%80 - 2 > 0){
     --pos;
     --back_counter;
    outb(CRTPORT+1, pos);
    }
    return;
  }else if (c == KEY_UP) {
        if (history_index > 0) {
            cprintf("\r%s", history[history_index % HISTORY_SIZE]); // Print the command from history
            history_index--; 
            pos = strlen(history[history_index % HISTORY_SIZE]); // Set cursor to end of command
        }
    } else if (c == KEY_Down) {
        if (history_index < history_count - 1) {
            cprintf("\r%s", history[history_index % HISTORY_SIZE]); // Print the command from history
            history_index++;
            pos = strlen(history[history_index % HISTORY_SIZE]); // Set c
        }
  }else if(c == BACKSPACE){
    if(pos > 0) --pos;
  } else {
        // Shift characters to the right to make space for the new character
        int end_pos = 24 * 80 - 1;  // The last position on the screen
        for (int i = end_pos; i >= pos; i--) {
            crt[i] = crt[i - 1];  // Shift characters one position to the right
        }

        crt[pos] = (c & 0xff) | 0x0700;  // Insert the new character
        pos++;
    }

  if(pos < 0 || pos > 25*80)
    panic("pos under/overflow");

  if((pos/80) >= 24){  // Scroll up.
    memmove(crt, crt+80, sizeof(crt[0])*23*80);
    pos -= 80;
    memset(crt+pos, 0, sizeof(crt[0])*(24*80 - pos));
  }

  outb(CRTPORT, 14);
  outb(CRTPORT+1, pos>>8);
  outb(CRTPORT, 15);
  outb(CRTPORT+1, pos);
  crt[pos] = ' ' | 0x0700;
}

void
consputc(int c)
{
  if(panicked){
    cli();
    for(;;)
      ;
  }

  if(c == BACKSPACE){
    uartputc('\b'); uartputc(' '); uartputc('\b');
  } else
    uartputc(c);
  cgaputc(c);
}

#define INPUT_BUF 128
struct {
  char buf[INPUT_BUF];
  uint r;  // Read index
  uint w;  // Write index
  uint e;  // Edit index
} input;

#define C(x)  ((x)-'@')  // Control-x


int is_operator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

int calculate_expression(char *expr, int len) {
    int num1 = 0, num2 = 0;
    char op = 0;  // Initialize operator to zero
    int i = 0;

    // Extract the first number
    while (i < len && expr[i] >= '0' && expr[i] <= '9') {
        num1 = num1 * 10 + (expr[i] - '0');
        i++;
    }

    // Get the operator
    if (i < len && is_operator(expr[i])) {
        op = expr[i++];
    } else {
        return -1;  // Invalid expression if no operator is found
    }

    // Extract the second number
    while (i < len && expr[i] >= '0' && expr[i] <= '9') {
        num2 = num2 * 10 + (expr[i] - '0');
        i++;
    }

    // Validate the remaining characters
    for (; i < len; i++) {
        if (expr[i] != ' ' && !is_operator(expr[i]) && (expr[i] < '0' || expr[i] > '9')) {
            return -1;  // Invalid character in expression
          }
    }

void itoa(int num, char *buf, int buf_size) {
    int i = 0;
    int is_negative = (num < 0);  // Determine if the number is negative

    if (is_negative) {
        num = -num;  // Convert to positive for processing
    }

    // Convert number to string
    do {
        if (i < buf_size - 1) {  // Ensure we don't overflow the buffer
            buf[i++] = (num % 10) + '0';
        }
        num /= 10;
    } while (num > 0);

    // Add negative sign if the number was negative
    if (is_negative && i < buf_size - 1) {
        buf[i++] = '-';
    }

    buf[i] = '\0';  // Null-terminate the string

    // Reverse the string to get the correct order
    for (int j = 0; j < i / 2; j++) {
        char temp = buf[j];
        buf[j] = buf[i - j - 1];
        buf[i - j - 1] = temp;
    }
}

void consoleintr(int (*getc)(void)) {
    int c, doprocdump = 0;
    static char expr_buf[32];
    static int expr_len = 0;
    char result_str[16];  // Buffer for storing the result of calculations
    static int cursor_pos = 0;  // Tracks the current cursor position in the buffer
    static char last_char = 0;  // To keep track of the last character input
    static int copy_mode = 0;   // Flag for copy mode
    static char copy_buffer[COPY_BUF_SIZE];  // Buffer for copied text
    static int copylen = 0;  // Length of copied text

    acquire(&cons.lock);
    while((c = getc()) >= 0) {
        switch(c) {
            case C('P'):  // Process listing.
                doprocdump = 1;
                break;
            case C('U'):  // Kill line.
                while(input.e != input.w && input.buf[(input.e - 1) % INPUT_BUF] != '\n') {
                    input.e--;
                    cursor_pos = input.e;  // Reset cursor position to end of buffer
                    consputc(BACKSPACE);
                }
                expr_len = 0;  // Clear the expression buffer
                last_char = 0;  // Reset last character
                break;
            case C('H'): case '\x7f':  // Backspace
                if(input.e != input.w) {
                    input.e--;
                    cursor_pos--;  // Move cursor position back
                    consputc(BACKSPACE);
                }
                if(expr_len > 0) expr_len--;  // Manage backspace in the input buffer
                last_char = 0;  // Reset last character
                break;
            case C('S'):  // Ctrl + S to start copying
                copy_mode = 1;  // Enter copy mode
                copylen = 0;    // Reset the copy buffer
                break;
            case C('F'):  // Ctrl + F to paste copied text
                for(int i = 0; i < copylen; i++) {
                    if(input.e - input.r < INPUT_BUF) {  // Ensure there's space in the input buffer
                        input.buf[input.e++ % INPUT_BUF] = copy_buffer[i];
                        consputc(copy_buffer[i]);
                    }
                }
                break;
            // Left arrow key handling
            case KEY_LEFT:  // Left arrow key
                cgaputc(c);
                break;
            // Right arrow key handling
            case KEY_RIGHT:  // Right arrow key
                cgaputc(c);
                break;
            // Up arrow key handling (you can add functionality if needed)
            case KEY_UP:
                cgaputc(c);
                break;
            default:
                if(c != 0 && input.e - input.r < INPUT_BUF) {
                    c = (c == '\r') ? '\n' : c;  // Normalize carriage return to newline

                    // If in copy mode, store the character in the copy buffer
                    if(copy_mode && copylen < COPY_BUF_SIZE) {
                        copy_buffer[copylen++] = c;
                    }

                    // Insert the new character at the cursor position
                    for(int i = input.e; i > cursor_pos; i--) {
                        input.buf[i % INPUT_BUF] = input.buf[(i - 1) % INPUT_BUF];
                    }
                    input.buf[cursor_pos % INPUT_BUF] = c;  // Insert the character
                    input.e++;  // Increment end index
                    cursor_pos++;  // Move cursor position to the right after insertion
                    consputc(c);

                    // Store valid characters in the calculation buffer
                    if(expr_len < sizeof(expr_buf) - 1) {
                        // Allow only digits, operators, and spaces
                        if ((c >= '0' && c <= '9') || is_operator(c) || c == ' ' || c == '=' || c == '?') {
                            expr_buf[expr_len++] = c;
                            expr_buf[expr_len] = 0;  // Null-terminate the string

                            // Check for the calculation trigger sequence "=?"
                            if(last_char == '=' && c == '?') {
                                int result = calculate_expression(expr_buf, expr_len - 2); // Remove '=' and '?' for calculation
                                if (result != -1) {
                                    // Clear the expression
                                    for (int i = 0; i < expr_len; i++) {
                                        consputc(BACKSPACE);
                                        input.e--;
                                    }

                                    // Convert the result to string
                                    itoa(result, result_str, sizeof(result_str));

                                    // Display the result
                                    for (int i = 0; result_str[i] != '\0'; i++) {
                                        input.buf[input.e++ % INPUT_BUF] = result_str[i];
                                        consputc(result_str[i]);
                                    }
                                }
                                expr_len = 0;  // Reset the buffer
                            }

                            // Update last character to the current one
                            last_char = c;
                        }
                    }

                    if(c == '\n' || c == C('D') || input.e == input.r + INPUT_BUF) {
                        input.w = input.e;
                        wakeup(&input.r);
                    }
                }
                break;
        }
    }
    release(&cons.lock);

    if(doprocdump) {
        procdump();  // Now call procdump() without holding cons.lock
    }
}


  }
  release(&cons.lock);

  if(doprocdump) {
    procdump();  // Now call procdump() without cons.lock held
  }
}



int
consoleread(struct inode *ip, char *dst, int n)
{
  uint target;
  int c;

  iunlock(ip);
  target = n;
  acquire(&cons.lock);
  while(n > 0){
    while(input.r == input.w){
      if(myproc()->killed){
        release(&cons.lock);
        ilock(ip);
        return -1;
      }
      sleep(&input.r, &cons.lock);
    }
    c = input.buf[input.r++ % INPUT_BUF];
    if(c == C('D')){  // EOF
      if(n < target){
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        input.r--;
      }
      break;
    }
    *dst++ = c;
    --n;
    if(c == '\n')
      break;
  }
  release(&cons.lock);
  ilock(ip);

  return target - n;
}

int
consolewrite(struct inode *ip, char *buf, int n)
{
  int i;

  iunlock(ip);
  acquire(&cons.lock);
  for(i = 0; i < n; i++)
    consputc(buf[i] & 0xff);
  release(&cons.lock);
  ilock(ip);

  return n;
}

void
consoleinit(void)
{
  initlock(&cons.lock, "console");

  devsw[CONSOLE].write = consolewrite;
  devsw[CONSOLE].read = consoleread;
  cons.locking = 1;

  ioapicenable(IRQ_KBD, 0);
}

