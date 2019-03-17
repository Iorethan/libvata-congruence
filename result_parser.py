timeout = 300.0
failure = 2
sep = ";"

import sys
if len(sys.argv) != 2:
    exit(1)


class Result(object):
    def __init__(self, anti, bisim, result):
        self.anti = anti
        self.bisim = bisim
        self.aver = (anti + bisim) / 2.0
        self.result = result
    
    def __repr__(self):
        return self.__str__()

    def __str__(self):
        return "[result: " + str(self.result) + ",\tatime: " + str(self.aver) + ",\tanti: " + str(self.anti) + ",\tbisim: " + str(self.bisim) + "]"


def lineToInt(line):
    result = []
    for item in line[:-1]:
        try:
            result.append(float(item))
        except:
            result.append(timeout)
    try:
        result.append(int(line[-1]))
    except:
        result.append(failure)

    return Result(result[0] if result[0] < result[1] else result[1], result[2], result[3])


def countScore(data, tolerance = None):
    id1 = 0
    cnt = len(data)
    result = ""
    for i in range(len(data[0]) - 2):
        id2 = i + 1
        better1 = 0
        better2 = 0
        same = 0
        for d in data:
            if tolerance == None:
                if d[id1] < d[id2]:
                    better1 += 1
                else:
                    better2 += 1
            else:
                if (abs(d[id1]/d[id2] - 1) < tolerance):
                    same += 1
                elif d[id1] < d[id2]:
                    better1 += 1
                else:
                    better2 += 1
        result += str(better1/cnt) + ";"
        result += str(better2/cnt) + ";"
        result += str(same/cnt) + "\n"
    return result

def print_percs(percs, data):
    print("percentile" + sep + "time" + sep)
    for perc in percs:
        print_perc(perc, data)

def print_perc(perc, data):
    print(str(perc) + sep + countPerc(perc, data) + sep)

def countPerc(perc, data):
    index = int(len(data) * perc) - 1
    return str(data[index])

def print_scatter_data(data):
    print("dificculity" + sep + "antichain" + sep + "bisimulation" + sep)
    for item in data:
        print_scatted_item(item.aver, item.anti, item.bisim)

def print_scatted_item(aver, anti, bisim):
    print(str(aver) + sep + str(anti) + sep + str(bisim) + sep)

with open(sys.argv[1]) as f:
    content = f.readlines()
    content = content[6:]

all_data = [lineToInt(x.split(";")[2:-2]) for x in content]
all_data_avg_sorted = sorted(all_data, key=lambda x: x.aver)
anti_sorted = sorted([x.anti for x in all_data_avg_sorted])
bisim_sorted = sorted([x.bisim for x in all_data_avg_sorted])

eq_data_avg_sorted = [x for x in all_data_avg_sorted if x.result == 1]
eq_anti_sorted = sorted([x.anti for x in eq_data_avg_sorted])
eq_bisim_sorted = sorted([x.bisim for x in eq_data_avg_sorted])

neq_data_avg_sorted = [x for x in all_data_avg_sorted if x.result == 0]
neq_anti_sorted = sorted([x.anti for x in neq_data_avg_sorted])
neq_bisim_sorted = sorted([x.bisim for x in neq_data_avg_sorted])

percs = [x / 10.0 for x in range(1, 10)] + [x / 100.0 for x in range(91, 101)]

print("")
print("Antichain percentiles")
print_percs(percs, anti_sorted)

print("")
print("Bisimulation percentiles")
print_percs(percs, bisim_sorted)

print("")
print("Antichain percentiles eq")
print_percs(percs, eq_anti_sorted)

print("")
print("Bisimulation percentiles eq")
print_percs(percs, eq_bisim_sorted)

print("")
print("Antichain percentiles neq")
print_percs(percs, neq_anti_sorted)

print("")
print("Bisimulation percentiles neq")
print_percs(percs, neq_bisim_sorted)

print("")
print("Scatter data all")
print_scatter_data(all_data_avg_sorted)

print("")
print("Scatter data eq")
print_scatter_data(eq_data_avg_sorted)

print("")
print("Scatter data neq")
print_scatter_data(neq_data_avg_sorted)
