{ pkgs }: {
	deps = [
		pkgs.clang_12
		pkgs.ccls
		pkgs.gdb
		pkgs.gnumake
	  pkgs.curl.dev
  ];
    env = {
    C_INCLUDE_PATH = "${pkgs.libxml2.dev}/include/libxml2";
  };
}