.PHONY: all

HTML_DIR   = html
DOCBOOKXSL = xsl/docbook/xsl
CUSTOM_XSL_HTML_CHUNK=$(PWD)/xsl/html_chunk.xsl

all:	mod_ruby

includes:
	./scripts/makeincludes.sh

mod_ruby: images xml/* xml/website.xml xml/
	cd $(HTML_DIR) && xsltproc -o $@ -path $(DOCBOOKDTD)   \
        --nonet                                                          \
	    --stringparam section.autolabel 1                                \
        --stringparam chunker.output.encoding UTF-8                      \
        --xinclude $(CUSTOM_XSL_HTML_CHUNK)                              \
        $(PWD)/xml/$@.xml

pdf: xml/* xml/website.xml xml/
	cd pdf && dblatex -p $(PWD)/dblatex/librement.xsl  \
       -s $(PWD)/dblatex/librement.sty                 \
       -o doc.pdf -b xetex -V $(PWD)/xml/vrtql.xml

