/php_gtk_ext_/ {
	if (old_filename != FILENAME) {
		printf "#include \"" FILENAME "\"@NEWLINE@"
		old_filename = FILENAME
	}
}
