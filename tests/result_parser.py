import sys
if len(sys.argv) != 2:
    exit(1)


def lineToInt(line):
    result = []
    for item in line[:-1]:
        try:
            result.append(float(item))
        except:
            result.append(5.0)
    try:
        result.append(int(line[-1]))
    except:
        result.append(2)
    return result

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

def countPerc(data, perc):
    result = ""
    ln = int(len(data) * perc)
    for i in range(len(data[0]) - 1):
        data.sort(key=lambda x: x[i])
        result += str(data[ln][i]) + ";"
    # for i in range(len(data[0]) - 2):
    #     result += str(result[i+1] / result[0])
    return result


with open(sys.argv[1]) as f:
    content = f.readlines()
    content = content[6:]


content = list(map(lambda x: lineToInt(x.split(";")[2:-2]), content))
print("all;" + str(countScore(content)))
print("0;" + str(countScore(list(filter(lambda x: x[-1] == 0, content)))))
print("1;" + str(countScore(list(filter(lambda x: x[-1] == 1, content)))))
# print("all;" + str(countPerc(content, 0.8)))
# print("all;" + str(countPerc(content, 0.85)))
# print("all;" + str(countPerc(content, 0.9)))
# print("all;" + str(countPerc(content, 0.95)))
# print("all;" + str(countPerc(content, 0.96)))
# print("all;" + str(countPerc(content, 0.97)))
# print("all;" + str(countPerc(content, 0.98)))
# print("all;" + str(countPerc(content, 0.99)))
# print("0;" + str(countPerc(list(filter(lambda x: x[-1] == 0, content)), 0.8)))
# print("0;" + str(countPerc(list(filter(lambda x: x[-1] == 0, content)), 0.85)))
# print("0;" + str(countPerc(list(filter(lambda x: x[-1] == 0, content)), 0.9)))
# print("0;" + str(countPerc(list(filter(lambda x: x[-1] == 0, content)), 0.95)))
# print("0;" + str(countPerc(list(filter(lambda x: x[-1] == 0, content)), 0.96)))
# print("0;" + str(countPerc(list(filter(lambda x: x[-1] == 0, content)), 0.97)))
# print("0;" + str(countPerc(list(filter(lambda x: x[-1] == 0, content)), 0.98)))
# print("0;" + str(countPerc(list(filter(lambda x: x[-1] == 0, content)), 0.99)))
# print("1;" + str(countPerc(list(filter(lambda x: x[-1] == 1, content)), 0.8)))
# print("1;" + str(countPerc(list(filter(lambda x: x[-1] == 1, content)), 0.85)))
# print("1;" + str(countPerc(list(filter(lambda x: x[-1] == 1, content)), 0.9)))
# print("1;" + str(countPerc(list(filter(lambda x: x[-1] == 1, content)), 0.95)))
# print("1;" + str(countPerc(list(filter(lambda x: x[-1] == 1, content)), 0.96)))
# print("1;" + str(countPerc(list(filter(lambda x: x[-1] == 1, content)), 0.97)))
# print("1;" + str(countPerc(list(filter(lambda x: x[-1] == 1, content)), 0.98)))
# print("1;" + str(countPerc(list(filter(lambda x: x[-1] == 1, content)), 0.99)))