import sys
import math
if len(sys.argv) != 2:
    exit(1)


def lineToInt(line):
    result = []
    for item in line[:-1]:
        try:
            if(math.isnan(float(item))):
                return None
            result.append(float(item))
        except:
            # result.append(5.0)
            return None
    try:
        result.append(int(line[-1]))
    except:
        result.append(2)
    rs = []
    if result[0] < result [1]:
        rs.append(result[0])
    else:
        rs.append(result[1])
    for i in range(len(result) - 2):
        rs.append(result[i + 2])
    return rs

def countScore(data, tolerance = None):
    cnt = len(data)
    result = ""
    for i in range(len(data[0]) - 1):
        sum_perc = 0
        for d in data:
            sum_perc += d[i]
        result += str(sum_perc/cnt) + ";"
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
content = list(filter(lambda x: x != None, content))
print("all;" + str(countScore(content)))
print("0;" + str(countScore(list(filter(lambda x: x[-1] == 0, content)))))
print("1;" + str(countScore(list(filter(lambda x: x[-1] == 1, content)))))

# print("all;" + str(countPerc(content, 0.1)))
# print("all;" + str(countPerc(content, 0.2)))
# print("all;" + str(countPerc(content, 0.3)))
# print("all;" + str(countPerc(content, 0.4)))
# print("all;" + str(countPerc(content, 0.5)))
# print("all;" + str(countPerc(content, 0.6)))
# print("all;" + str(countPerc(content, 0.7)))
# print("all;" + str(countPerc(content, 0.8)))
# print("all;" + str(countPerc(content, 0.9)))
# print("all;" + str(countPerc(content, 0.95)))
# print("0;" + str(countPerc(list(filter(lambda x: x[-1] == 0, content)), 0.1)))
# print("0;" + str(countPerc(list(filter(lambda x: x[-1] == 0, content)), 0.2)))
# print("0;" + str(countPerc(list(filter(lambda x: x[-1] == 0, content)), 0.3)))
# print("0;" + str(countPerc(list(filter(lambda x: x[-1] == 0, content)), 0.4)))
# print("0;" + str(countPerc(list(filter(lambda x: x[-1] == 0, content)), 0.5)))
# print("0;" + str(countPerc(list(filter(lambda x: x[-1] == 0, content)), 0.6)))
# print("0;" + str(countPerc(list(filter(lambda x: x[-1] == 0, content)), 0.7)))
# print("0;" + str(countPerc(list(filter(lambda x: x[-1] == 0, content)), 0.8)))
# print("0;" + str(countPerc(list(filter(lambda x: x[-1] == 0, content)), 0.9)))
# print("0;" + str(countPerc(list(filter(lambda x: x[-1] == 0, content)), 0.95)))
# print("1;" + str(countPerc(list(filter(lambda x: x[-1] == 1, content)), 0.1)))
# print("1;" + str(countPerc(list(filter(lambda x: x[-1] == 1, content)), 0.2)))
# print("1;" + str(countPerc(list(filter(lambda x: x[-1] == 1, content)), 0.3)))
# print("1;" + str(countPerc(list(filter(lambda x: x[-1] == 1, content)), 0.4)))
# print("1;" + str(countPerc(list(filter(lambda x: x[-1] == 1, content)), 0.5)))
# print("1;" + str(countPerc(list(filter(lambda x: x[-1] == 1, content)), 0.6)))
# print("1;" + str(countPerc(list(filter(lambda x: x[-1] == 1, content)), 0.7)))
# print("1;" + str(countPerc(list(filter(lambda x: x[-1] == 1, content)), 0.8)))
# print("1;" + str(countPerc(list(filter(lambda x: x[-1] == 1, content)), 0.9)))
# print("1;" + str(countPerc(list(filter(lambda x: x[-1] == 1, content)), 0.95)))
