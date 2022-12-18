from typing import Any, Callable, Iterable, Optional


ROCKS: list[list[complex]] = [
    [0, 1,    2,    3],
    [1, 0+1j, 2+1j, 1+2j],
    [0, 1,    2,    2+1j, 2+2j],
    [0, 0+1j, 0+2j, 0+3j],
    [0, 1,    0+1j, 1+1j]
]


def do_simulation(
        rocks: Iterable[tuple[int, tuple[list[complex], int]]],
        jets: Iterable[tuple[int, int]],
        limit: int
) -> tuple[int, int]:
    tower: set[complex] = set()
    cache: dict[tuple[int, int], tuple[int, int]] = dict()
    top: int = 0
    p1: Optional[int] = None

    def check_placement(pos: complex, dir: complex, rock: list[complex]) -> bool:
        def valid(pos: complex) -> bool:
            return 0 <= pos.real < 7 and pos.imag > 0 and pos not in tower
        return all(valid(pos + dir + r) for r in rock)

    for step in range(1, limit + 1):
        pos: complex = complex(2, top + 4)
        (rock_idx, (rock, piece_top)) = next(rocks)
        while True:
            jet_idx, jet = next(jets)
            if check_placement(pos, jet, rock):
                pos += jet
            if check_placement(pos, -1j, rock):
                pos += -1j
            else:
                break
        tower |= {pos + r for r in rock}
        top = max(top, int(pos.imag) + piece_top)

        if step == 2022:
            p1 = top

        key: tuple[int, int] = (rock_idx, jet_idx)
        if key in cache:
            cstep, ctop = cache[key]
            match divmod(limit - step, step - cstep):
                case d, 0:
                    p2 = top + (top - ctop) * d
                    return p1, p2
        else:
            cache[key] = step, top


def forever(
        gen: Iterable[Any],
        fn: Callable[[Any], Any] = lambda x: x
    ) -> Iterable[tuple[int, Any]]:
    while True:
        for i, x in enumerate(gen):
            yield i, fn(x)


def rock_top(r: list[complex]) -> tuple[list[complex], int]:
    return r, max(int(c.imag) for c in r)


def main():
    rocks: Iterable[tuple[int, tuple[list[complex], int]]] = forever(ROCKS, rock_top)
    jets: Iterable[tuple[int, int]] = forever([ord(x) - ord('=') for x in open(0).readlines()[0][:-1]])
    p1, p2 = do_simulation(rocks, jets, 1_000_000_000_000)
    print(f"Part 1: {p1}\nPart 2: {p2}")


if __name__ == '__main__':
    main()
