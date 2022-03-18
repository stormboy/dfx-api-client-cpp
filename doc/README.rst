===
doc
===

Documentation is generated with Sphinx and relies on Doxygen and the Breathe bridge which offers
rich documentation capabilities while keeping the documentation integrated with the project.

Sphinx utilizes reStructuredText format and has been integrated with sphinx-intl to leverage
gettext for managing locale variants of the \*.rst sources.

The Doxygen sources are handled slightly differently and involve embedding the language tag
in the headers to toggle between languages.

When building WITH_DOCS, the language can be changed to "chinese" or "english" (default). The corresponding
documentation will be generated with the appropriate language substitutions.
