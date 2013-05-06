default:
	@echo ""
	@echo "This is the root makefile for team Awesome (aka team Java Package)'s midterm!"
	@echo "Use one of the following commands:"
	@echo "-----------------------------------------------------------------------------"
	@echo "  $(MAKE) win32              to compile for Windows using MinGW"
	@echo "  $(MAKE) win32-clean        to remove any files generated for this target"
	@echo "-----------------------------------------------------------------------------"
	@echo "  $(MAKE) x11                to compile for X11 on Unix-like systems"
	@echo "  $(MAKE) x11-clean          to remove any files generated for this target"
	@echo "-----------------------------------------------------------------------------"

win32:
	$(MAKE) -f Makefile.win32
win32-clean:
	$(MAKE) -f Makefile.win32 clean
  
x11:
	$(MAKE) -f Makefile.x11
x11-clean:
	$(MAKE) -f Makefile.x11 clean
