# -*- Makefile -*-
# Eugene Skepner 2017
# ----------------------------------------------------------------------

MAKEFLAGS = -w

# ----------------------------------------------------------------------

SOURCES = chart-base.cc chart.cc chart-plot-spec.cc bounding-ball.cc layout-base.cc layout.cc ace.cc lispmds.cc
PY_SOURCES = py.cc $(SOURCES)

ACMACS_CHART_LIB_MAJOR = 1
ACMACS_CHART_LIB_MINOR = 0
ACMACS_CHART_LIB_NAME = libacmacschart
ACMACS_CHART_LIB = $(DIST)/$(call shared_lib_name,$(ACMACS_CHART_LIB_NAME),$(ACMACS_CHART_LIB_MAJOR),$(ACMACS_CHART_LIB_MINOR))

ACMACS_CHART_PY_LIB_MAJOR = 1
ACMACS_CHART_PY_LIB_MINOR = 0
ACMACS_CHART_PY_LIB_NAME = acmacs_chart_backend
ACMACS_CHART_PY_LIB = $(DIST)/$(ACMACS_CHART_PY_LIB_NAME)$(PYTHON_MODULE_SUFFIX)

# ----------------------------------------------------------------------

include $(ACMACSD_ROOT)/share/makefiles/Makefile.g++
include $(ACMACSD_ROOT)/share/makefiles/Makefile.python
include $(ACMACSD_ROOT)/share/makefiles/Makefile.dist-build.vars

CXXFLAGS = -g -MMD $(OPTIMIZATION) $(PROFILE) -fPIC -std=$(STD) $(WARNINGS) -Icc -I$(AD_INCLUDE) $(PKG_INCLUDES)
LDFLAGS = $(OPTIMIZATION) $(PROFILE)
LDLIBS = \
	$(AD_LIB)/$(call shared_lib_name,libacmacsbase,1,0) \
	$(AD_LIB)/$(call shared_lib_name,liblocationdb,1,0) \
	$$(pkg-config --libs liblzma) $(CXX_LIB)

PKG_INCLUDES = $(shell pkg-config --cflags liblzma) $(shell $(PYTHON_CONFIG) --includes)

# ----------------------------------------------------------------------

all: check-acmacsd-root install-headers $(ACMACS_CHART_LIB) $(ACMACS_CHART_PY_LIB)

install: check-acmacsd-root install-headers $(ACMACS_CHART_LIB) $(ACMACS_CHART_PY_LIB)
	$(call install_lib,$(ACMACS_CHART_LIB))
	$(call install_py_lib,$(ACMACS_CHART_PY_LIB))
	ln -sf $(abspath py)/* $(AD_PY)
	@#ln -sf $(abspath bin)/acmacs-chart-* $(AD_BIN)

test: install
	test/test

# ----------------------------------------------------------------------

-include $(BUILD)/*.d
include $(ACMACSD_ROOT)/share/makefiles/Makefile.dist-build.rules
include $(ACMACSD_ROOT)/share/makefiles/Makefile.rtags

# ----------------------------------------------------------------------

$(ACMACS_CHART_LIB): $(patsubst %.cc,$(BUILD)/%.o,$(SOURCES)) | $(DIST) $(LOCATION_DB_LIB)
	@printf "%-16s %s\n" "SHARED" $@
	@$(call make_shared,$(ACMACS_CHART_LIB_NAME),$(ACMACS_CHART_LIB_MAJOR),$(ACMACS_CHART_LIB_MINOR)) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(ACMACS_CHART_PY_LIB): $(patsubst %.cc,$(BUILD)/%.o,$(PY_SOURCES)) | $(DIST)
	@printf "%-16s %s\n" "SHARED" $@
	@$(call make_shared,$(ACMACS_CHART_PY_LIB_NAME),$(ACMACS_CHART_PY_LIB_MAJOR),$(ACMACS_CHART_PY_LIB_MINOR)) $(LDFLAGS) -o $@ $^ $(LDLIBS) $(PYTHON_LDLIBS)

# ======================================================================
### Local Variables:
### eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
### End:
