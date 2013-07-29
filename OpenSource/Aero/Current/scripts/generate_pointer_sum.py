# coding: utf-8

import sys, re, copy, pickle, os

def gen_template(n):
    print(r"template <typename ReturnType, typename PType", end = "")

    for i in range(0, n):
        print(r", typename A{0}".format(i), end = "")

    print(r">")

def gen_decl(fn, n):
    print(r"inline ReturnType {0}(PType p".format(fn), end = "")

    for i in range(0, n):
        print(r", A{0} a{0}".format(i), end = "")

    print(r")")

def gen_return(n):
    print("\treturn (ReturnType)((boost::uint8_t *)p", end = "")

    for i in range(0, n):
        print(r" + ((intptr_t)a{0})".format(i), end = "")

    print(r");")

def gen_psum(fn, n):
    gen_template(n)
    gen_decl(fn, n)
    print("{")
    gen_return(n)
    print("}")
    print()

if __name__ == "__main__":
    amount = int(sys.argv[1])

    for i in range(1, amount):
        gen_psum("p_sum", i)
