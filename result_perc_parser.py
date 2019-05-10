timeout = 300.0
failure = 2
sep = ";"

import sys
if len(sys.argv) != 3:
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

def print_percs(data, data_p):
    print("bisimulation" + sep + "antichains" + sep)
    cnt_all = sum([item.bisim for item in data])
    cnt_pruned = sum([item.bisim for item in data_p])
    cnt_all_a = sum([item.anti for item in data])
    cnt_pruned_a = sum([item.anti for item in data_p])
    print(str(cnt_pruned/cnt_all) + sep + str(cnt_pruned_a/cnt_all_a) + sep)

with open(sys.argv[1]) as f:
    content = f.readlines()
    content = content[6:]

all_data = [lineToInt(x.split(";")[2:-2]) for x in content]
eq_data = [x for x in all_data if x.result == 1]
neq_data = [x for x in all_data if x.result == 0]

with open(sys.argv[2]) as f:
    content = f.readlines()
    content = content[6:]

all_data_p = [lineToInt(x.split(";")[2:-2]) for x in content]
eq_data_p = [x for x in all_data_p if x.result == 1]
neq_data_p = [x for x in all_data_p if x.result == 0]

print("")
print("Skipped percentage all")
print_percs(all_data, all_data_p)

print("")
print("Skipped percentage eq")
print_percs(eq_data, eq_data_p)

print("")
print("Skipped percentage neq")
print_percs(neq_data, neq_data_p)
