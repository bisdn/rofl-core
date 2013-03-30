
if doc

doc: all-local
all-local:
	@echo -n Generating documenation of ROFL \(this may take a while\)...
	@ SRC_DIR=$(top_srcdir) @DOXYGEN@ doxyfile.conf >/dev/null 2>&1
	@echo done
	
	@echo -n Generating documenation of ROFL-pipeline \(this may take a while\)...
	@ SRC_DIR=$(top_srcdir) @DOXYGEN@ doxyfile-pipeline.conf >/dev/null 2>&1
	@echo done
	
	@echo -n Generating documenation of ROFL-afa \(this may take a while\)...
	@ SRC_DIR=$(top_srcdir) @DOXYGEN@ doxyfile-afa.conf >/dev/null 2>&1
	@echo done
	
	@echo  The documenation is under: $(top_builddir)/doc/ 

endif
