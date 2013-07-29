# coding: utf-8

import sys, re, copy, pickle, os

def gen_template(n):
    print(r"template <typename ReturnType, typename FPType", end = "")

    for i in range(0, n):
        print(r", typename A{0}".format(i), end = "")

    print(r">")

def gen_decl(fn, n):
    print(r"inline ReturnType {0}(FPType fp".format(fn), end = "")

    for i in range(0, n):
        print(r", A{0} a{0}".format(i), end = "")

    print(r")")

def gen_fp_decl(cv, n):
    print("\ttypedef ReturnType ({0} *TFPAeroFunction)(".format(cv), end = "")

    for i in range(0, n):
        if i == 0:
            print(r"A{0} a{0}".format(i), end = "")
        else:
            print(r", A{0} a{0}".format(i), end = "")

    print(r");")

def gen_return(n):
    print("\treturn ((TFPAeroFunction)fp)(", end = "")

    for i in range(0, n):
        if i == 0:
            print(r"a{0}".format(i), end = "")
        else:
            print(r", a{0}".format(i), end = "")

    print(r");")

def gen_fp(fn, cv, n):
    gen_template(n)
    gen_decl(fn, n)
    print("{")
    gen_fp_decl(cv, n)
    print()
    gen_return(n)
    print("}")
    print()

def gen_thiscall_fp_decl(cv, n):
    print("\ttypedef ReturnType ({0} *TFPAeroFunction)(".format(cv), end = "")

    for i in range(0, n):
        if i == 0:
            print(r"A{0} a{0}, void *".format(i), end = "")
        else:
            print(r", A{0} a{0}".format(i), end = "")

    print(r");")

def gen_thiscall_return(n):
    print("\treturn ((TFPAeroFunction)fp)(", end = "")

    for i in range(0, n):
        if i == 0:
            print(r"a{0}, 0".format(i), end = "")
        else:
            print(r", a{0}".format(i), end = "")

    print(r");")


def gen_this_call(fn, n):
    gen_template(n)
    gen_decl(fn, n)
    print("{")
    gen_thiscall_fp_decl("AERO_FP_CALL_FASTCALL", n)
    print()
    gen_thiscall_return(n)
    print("}")
    print()

callConvertion = {"AERO_FP_CALL_CDECL" : "c_call", "AERO_FP_CALL_STDCALL" : "std_call", "AERO_FP_CALL_FASTCALL" : "fast_call", "" : "native_call"}

if __name__ == "__main__":
    amount = int(sys.argv[1])

    for cv in callConvertion:
        fn = callConvertion[cv]
        for i in range(0, amount):
            gen_fp(fn, cv, i)

    for i in range(1, amount):
        gen_this_call("this_call", i)
