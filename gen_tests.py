import random
from numpy import base_repr


NUM_TESTS = int(1e6)

with open("tests/gen1.txt", "w") as file:
    for _ in range(NUM_TESTS):
        op = random.choice(("+", "*"))
        base = random.randint(2, 16)
        arg1 = base_repr(random.randint(0, 16**40), base)[:40]
        arg2 = base_repr(random.randint(0, 16**40), base)[:40]

        file.write(f"{op} {base}\n")
        file.write(f"{arg1}\n{arg2}\n\n")
