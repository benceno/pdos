import re
import matplotlib.pyplot as plt

def parse_data(data):
    process_data = {}
    lines = data.split('\n')
    sanity_seen = False
    turnaround_seen = False

    for line in lines:
        if "sanity" in line:
            sanity_seen = True
        if "turnaround" in line:
            turnaround_seen = True
            # Pegar o começo do sanity antes das médias
        if sanity_seen and not turnaround_seen:
            if "pid" in line:
                match = re.search(r'pid: (\d+), type: (\w+), retime: (\d+), rutime: (\d+), stime: (\d+)', line)
                if match:
                    pid = int(match.group(1))
                    ptype = match.group(2)
                    retime = int(match.group(3))
                    rutime = int(match.group(4))
                    stime = int(match.group(5))
                    if ptype not in process_data:
                        process_data[ptype] = {'retime': [], 'rutime': [], 'stime': []}
                    process_data[ptype]['retime'].append(retime)
                    process_data[ptype]['rutime'].append(rutime)
                    process_data[ptype]['stime'].append(stime)

    return process_data

def save_plot(type):
    plt.xlabel('Process - Ordered by Conclusion Time')
    plt.ylabel(type)
    plt.title(type+' Metrics')
    plt.legend()
    plt.savefig(f'images/{type}_metrics.png')


def plot_type(process_data, type):
    plt.figure()
    for ptype, metrics in process_data.items():
        plt.plot(metrics[type], label=ptype)
    save_plot(type)

def plot_iobound_stime(process_data):
    plt.figure()
    for ptype, metrics in process_data.items():
        if ptype == 'IO_BOUND':
            plt.plot(metrics['stime'], label=ptype)

    save_plot('stime_iobound')
with open('output.txt', 'r') as file:
    data = file.read()

process_data = parse_data(data)

plot_type(process_data, 'retime')
plot_type(process_data, 'rutime')
plot_type(process_data, 'stime')
plot_iobound_stime(process_data)