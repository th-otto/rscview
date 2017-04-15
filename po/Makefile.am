include $(top_srcdir)/config.mk

XGETTEXT = xgettext
XGETTEXT_KEYWORDS = --keyword=_ --keyword=N_ 
MSGMERGE = msgmerge --no-fuzzy-matching --width=100

BUILT_SOURCES = aes.pot aes_de.po

aes.pot: ../aes/gem_rsc.rsh
	$(XGETTEXT) \
		--add-comments \
		--directory=. \
		--default-domain=aes \
		--language=C \
		--from-code=iso-8859-1 \
		$(XGETTEXT_KEYWORDS) \
		--output=$@ \
		$^

aes_de.po: aes.pot de.po
	$(MSGMERGE) -o $@ de.po $<
