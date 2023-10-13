rockspec_format = "3.0"
package = "kame-lua"
version = "dev-1"
source = {
    url = "git+https://github.com/bggd/kame"
}
description = {
    homepage = "https://github.com/bggd/kame",
    license = "MIT"
}
build = {
    type = "make",
    build_variables = {
        CFLAGS="$(CFLAGS)",
        LIBFLAG="$(LIBFLAG)",
        LUALIB="$(LUALIB)",
        LUA_LIBDIR="$(LUA_LIBDIR)",
        LUA_INCDIR="$(LUA_INCDIR)",
        INST_PREFIX = "$(PREFIX)"
    }
}

