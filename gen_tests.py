import random
from numpy import base_repr


NUM_TESTS = int(1e3)

with open("rand.txt", "w") as file:
    for _ in range(NUM_TESTS):
        op = random.choice(("+", "*", "/", "%", "^"))

        arg_len = random.randint(1, 40)
        base = random.randint(2, 16)

        arg1 = "".join(base_repr(random.randint(0, 16), base)
                       for _ in range(arg_len))[:arg_len]
        arg_len = random.randint(1, 3) if op == "^" else random.randint(1, 40)
        arg2 = "".join(base_repr(random.randint(0, 16), base)
                       for _ in range(arg_len))[:arg_len]

        if op in ("/", "%") and int(arg2, base=16) == 0:
            arg2 = "1"

        file.write(f"{op} {base}\n\n")
        file.write(f"{arg1}\n\n{arg2}\n\n\n\n")
