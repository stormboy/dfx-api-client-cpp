# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))

import os
import sphinx_rtd_theme
import rst2pdf.pdfbuilder

# -- Project information -----------------------------------------------------

project = 'DFX Cloud API'
copyright = '2022, Nuralogix Inc.'
author = 'Nuralogix'

root_doc = "index"

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    "sphinx_rtd_theme",
#    'sphinx.ext.pngmath',
    'sphinx.ext.todo',
    'sphinx.ext.imgconverter',
    'breathe'
    #,'rst2pdf.pdfbuilder'
]

cmake_binary_dir=os.getenv('CMAKE_BINARY_DIR', '../../../cmake-build-debug')
breathe_projects = { "CloudAPI": "{}/api-cpp/doc/docs/doxygen/xml".format(cmake_binary_dir)}

# Breathe Configuration
breathe_default_project = "CloudAPI"

# for sphinx-intl
locale_dirs = ['locale/']
gettext_compact = False     # optional

# inside conf.py
latex_documents = [('index', 'DFXCloud.tex', 'Cloud API Client C++ Documentation',
                    'Nuralogix', 'manual', 1)]
latex_logo = '_static/deepaffex-logo.png'   # svg imgconverter extension not working on logo
latex_elements = {
    'fontenc': r'\usepackage[LGR,X2,T1]{fontenc}',
    'passoptionstopackages': r'''
\PassOptionsToPackage{svgnames}{xcolor}
''',
    'preamble': r'''
\DeclareUnicodeCharacter{229E}{\ensuremath{\boxplus}}
\setcounter{tocdepth}{3}%    depth of what main TOC shows (3=subsubsection)
\setcounter{secnumdepth}{1}% depth of section numbering
''',
    # fix missing index entry due to RTD doing only once pdflatex after makeindex
    'printindex': r'''
\IfFileExists{\jobname.ind}
             {\footnotesize\raggedright\printindex}
             {\begin{sphinxtheindex}\end{sphinxtheindex}}
''',
   'extraclassoptions': 'openany,oneside',
}

latex_show_urls = 'footnote'
latex_use_xindy = True

# https://rst2pdf.org/static/manual.html#sphinx
# rst2pdf is not easy to configure via Sphinx via CMake from a git repository.
#
# To work around, I pass environment variables and then inject them values as
# options. The alternative equally ugly would have been to configure_file()
# this explicitly into the build folder with the settings - but then it would
# have had to copy all the file from here. This was the lesser of evils.
# breathe is definitely easier to work with - it picks some values via define.
#
# See the section in CMakeLists.txt which builds SphinxPDF and passes these
# as environment variables... maybe I should have hard-coded them but
# pdf_language was something which needed this setup.
pdf_use_coverpage = os.getenv('pdf_use_coverpage', 'True').lower() == 'true'
pdf_language = os.getenv('pdf_language', 'en_US')
pdf_style_path = os.getenv('pdf_style_path', None)
pdf_stylesheets = os.getenv('pdf_stylesheets', None)
pdf_use_index = os.getenv('pdf_use_index', False)
if pdf_style_path is not None:
    pdf_style_path = pdf_style_path.split(",")
if pdf_stylesheets is not None:
    pdf_stylesheets = pdf_stylesheets.split(",")

# While pdf_cover_template looks like an option, it is not configurable via this route and always
# defaults to sphinxcover.tmpl but the search path of rst2pdf takes templates from our source
# directory [doc/sphinx, ~/.rst2pdf, site-packages/rst2pdf/templates, doc/sphinx/_templates]
# which gives us one option to update, and explains why doc/sphinx/sphinxcover.tmpl exists.
rst2pdf_opts = {
    "pdf_use_coverpage": pdf_use_coverpage,
    "pdf_language": pdf_language,
    "pdf_style_path": pdf_style_path,
    "pdf_stylesheets": pdf_stylesheets,
    "pdf_use_toc": True,
}

# Grouping the document tree into PDF files. List of tuples
# (master document, name of pdf, title, author, options).
pdf_documents = [
    ('index', 'DFX Cloud', 'DFX Cloud', 'Nuralogix', rst2pdf_opts),
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = []


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_rtd_theme'

# spinx_rtd_theme configuration
html_show_sphinx = False
html_show_sourcelink = False

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

html_css_files = ['css/custom.css']


rst_prolog = """
 .. include:: <s5defs.txt>
 
 .. role:: logofont

 """
