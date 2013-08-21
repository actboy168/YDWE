/*
    cj_types_priv.j

    Basic replacement of JASS2 types to their C analogs.
    Private version, recommended for usage in imported libraries etc

    This file is part of cJass standard library
    © 2009 ADOLF \\ cjass.xgm.ru
*/

define {
    private int   = integer
    private bool  = boolean
    private void  = nothing
    private float = real
}