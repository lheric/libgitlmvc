# Sphinx configuration for the libgitlmvc documentation.
# Build locally:  sphinx-build -W docs docs/_build/html

project = "libgitlmvc"
author = "Huang Li"
copyright = "2013, Huang Li, IIPL"

extensions = [
    "myst_parser",          # write the pages in Markdown
    "sphinx_copybutton",    # copy button on code blocks
]

# Markdown pages; generate anchors for headings so pages can link to sections
source_suffix = {".md": "markdown"}
myst_heading_anchors = 3

# files in docs/ that are not part of the website
exclude_patterns = ["_build", "requirements.txt"]

html_theme = "furo"
html_title = "libgitlmvc"
html_theme_options = {
    "source_repository": "https://github.com/lheric/libgitlmvc/",
    "source_branch": "master",
    "source_directory": "docs/",
}
