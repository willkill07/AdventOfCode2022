import functools, operator, re, z3
L = list(map(functools.partial(re.findall, r'[^: \n]+'), open(0).readlines()))
V = {x: z3.Int(x) for x in set(map(operator.itemgetter(0), L))}
def solve(target):
  s = z3.Solver()
  for line in L:
    match line:
      case ['humn', v] if target != 'humn': s.add(V['humn'] == int(v))
      case [n, v] if n != 'humn': s.add(V[n] == int(v))
      case ['root', l, _, r] if target != 'root': s.add(V[l] == V[r])
      case [n, l, '+', r]: s.add(V[n] == V[l] + V[r])
      case [n, l, '-', r]: s.add(V[l] == V[n] + V[r])
      case [n, l, '*', r]: s.add(V[n] == V[l] * V[r])
      case [n, l, '/', r]: s.add(V[l] == V[n] * V[r])
  s.check()
  print("{}: {}".format(target, s.model().eval(V[target])))
solve('root')
solve('humn')