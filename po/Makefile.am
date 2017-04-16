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

emudesk::
	$(RM) desktop.rso desktop.rsc desktop.def LINGUAS
	wget --no-check-certificate https://raw.githubusercontent.com/emutos/emutos/master/desk/desktop.rsc
	wget --no-check-certificate https://raw.githubusercontent.com/emutos/emutos/master/desk/desktop.def
	wget --no-check-certificate https://raw.githubusercontent.com/emutos/emutos/master/po/LINGUAS
	LINGUAS=`cat LINGUAS | sed -e '/^#/d' -e '/^$$/d' -e 's/ .*//'`; \
	for lang in $$LINGUAS; do \
		$(RM) $$lang.po; \
		wget --no-check-certificate https://raw.githubusercontent.com/emutos/emutos/master/po/$$lang.po; \
	done; \
	for lang in en $$LINGUAS; do \
		../src/rscview --lang $$lang desktop.rsc; \
		test -d /srv/www/htdocs/rscview/$$lang || mkdir /srv/www/htdocs/rscview/$$lang; \
		cp -a *.png /srv/www/htdocs/rscview/$$lang; \
		test -d /home/sebilla/webgo24/home/www/rscview/$$lang || mkdir /home/sebilla/webgo24/home/www/rscview/$$lang; \
		mv *.png /home/sebilla/webgo24/home/www/rscview/$$lang; \
	done
