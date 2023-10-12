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
   type = "cmake",
   variables = {
       CMAKE_INSTALL_PREFIX = "$(PREFIX)"
   }
}

