{ pkgs }: {
	deps = [
		pkgs.clang_12
		pkgs.ccls
		pkgs.gdb
		pkgs.gnumake
	  pkgs.curl.dev
    pkgs.libcurl.dev
    pkgs.libxml.dev
  ];
}