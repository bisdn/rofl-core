
if doc

doc: all-local
all-local:
	@echo -n Compiling documenation of ROFL...
	@ @DOXYGEN@ doxyfile.conf >/dev/null 2>&1
	@echo done
	
	@echo -n Compiling documenation of  ROFL-pipeline...
	@ @DOXYGEN@ doxyfile-pipeline.conf >/dev/null 2>&1 
	@echo done

endif
