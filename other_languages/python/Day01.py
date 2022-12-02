with open('input/day01.txt', 'r') as f:
    elves = sorted([sum(map(int,e.split('\n'))) for e in f.read().split('\n\n')])
    print(elves[-1], sum(elves[-3:]))