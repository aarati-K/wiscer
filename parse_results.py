from tabulate import tabulate

try:
    f = open('results.txt', 'r')
except:
    print "File results.txt not found"
    quit()

head = ["Workload",
    "Chained Hashing (Mops/s)",
    "VIP Hashing (Mops/s)",
    "Gain VIP vs Chained hashing"]

table = []
row = []
for line in f:
    splits = line.split("Workload: ")
    if len(splits) > 1:
        if len(row):
            # Calculate gain
            gain = (row[2] - row[1])*100/row[1]
            row.append("{0:.1f}%".format(gain))
            table.append(row)
            row = []
        workload_name = splits[1]
        row.append(workload_name)
        continue

    splits = line.split("Net throughput: ")
    if len(splits) > 1:
        throughput = float(splits[1].split(" ")[0])
        row.append(throughput)

if len(row):
    gain = (row[2] - row[1])*100/row[1]
    row.append("{0:.1f}%".format(gain))
    table.append(row)

print(tabulate(table, headers=head, tablefmt="grid"))
