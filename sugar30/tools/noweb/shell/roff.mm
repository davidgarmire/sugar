.BEGINNINGOFDOCUMENT
.de CLIST
.CLISTBEGIN
.CITEM "action for \*[BEGINCONVQUOTE]tags\*[ENDCONVQUOTE] line" " 11c" " 12b 13a"
.CITEM "awk program" " 2c 12b" " 2b"
.CITEM "checkcode" " 10b" " 8c 9a 9c"
.CITEM "Example run" " 14b" ""
.CITEM "functions" " 8a 12a" " 2c 13a"
.CITEM "invoke awk program" " 2b" " 1a"
.CITEM "noroff" " 13a" ""
.CITEM "plant newline trap" " 6b" " 6a 7b"
.CITEM "process arguments" " 2a" " 1a"
.CITEM "read and process noweb source" " 2d 3b 3d 5c 5d 5e 6c 7a 8c 9a 9c 11a 11b" " 2c"
.CITEM "read back code chunk from diversion" " 5a" " 4a"
.CITEM "restore environment" " 5b" " 4a"
.CITEM "save environment" " 4c" " 4a"
.CITEM "set local environment" " 4d" " 4a"
.CITEM "start document" " 2f" " 2d"
.CITEM "tmac.w" " 2e 3a 3c 3e 4a 4b 6a 6d 7b 8b 8d 9b 9d 10a 10c 11d 13b 14a" ""
.CITEM "toroff" " 1a" ""
.CLISTEND
..
.de ILIST
.ILISTBEGIN
.IITEM "awkfile" ""
.IITEM "chunkdefn" ""
.IITEM "chunkuse" ""
.IITEM "code" ""
.IITEM "convquote" ""
.IITEM "defn" ""
.IITEM "delay" ""
.IITEM "entryarray" ""
.IITEM "entrydefn" ""
.IITEM "entryuse" ""
.IITEM "lastdefnlabel" ""
.IITEM "lastuse" ""
.IITEM "lastxreflabel" ""
.IITEM "lastxrefref" ""
.IITEM "macrodir" ""
.IITEM "name" ""
.IITEM "noindex" ""
.IITEM "tag" ""
.IITEM "tags" ""
.IITEM "tagsfile" ""
.IITEM "text" ""
.ILISTEND
..
.BEGINDOCCHUNK
.nr N 4
.nr O 1i
.nr P 2
.nr W 6.5i
.S 18 18
'ce
Converting noweb markup to troff markup
.sp 0.5i
.S 12 15
.PH "'Converting noweb markup to troff markup''%'"
.P
Toroff is a noweb backend for formatting text with troff
markup.  Toroff was written by Phil Bewig (\c
.BEGINQUOTEDCODE
\&pbewig@netcom.com\c
.ENDQUOTEDCODE
\&)
during April, 1996, and contributed to Norman Ramsey's noweb
literate programming system.  Liam Quin (\c
.BEGINQUOTEDCODE
\&lee@sq.com\c
.ENDQUOTEDCODE
\&) provided technical
assistance with troff.
Norman Ramsey made the code easier to port and made it conform with
his idea of the noweb philosophy.
Arnold Robbins (\c
.BEGINQUOTEDCODE
\&arnold@gnu.org\c
.ENDQUOTEDCODE
\&) modified it for use with
GNU troff (hopefully improving portability even more),
improved the flexibility of the noroff script,
and added extra explanation about the gory troff details.
.P
The high-level view of toroff is that it comes in two parts.
The first part is an ordinary back end, which produces 
troff augmented with special comments, using the \c
.BEGINQUOTEDCODE
\&.tm\c
.ENDQUOTEDCODE
\& macro.
The second part, \c
.BEGINQUOTEDCODE
\&noroff\c
.ENDQUOTEDCODE
\&, runs troff, with all of its
pre-processors and post-processors.
\c
.BEGINQUOTEDCODE
\&noroff\c
.ENDQUOTEDCODE
\& also processes the
comments in to a \c
.BEGINQUOTEDCODE
\&tags\c
.ENDQUOTEDCODE
\& file.
Moreover, \c
.BEGINQUOTEDCODE
\&noroff\c
.ENDQUOTEDCODE
\& gathers from the tags file the
cross-referencing information gathered on the previous formatting run.
The trick that makes the whole thing work is that the
troff markup added by toroff causes cross-referencing
information to be written to standard error during the formatting run;
the \c
.BEGINQUOTEDCODE
\&noroff\c
.ENDQUOTEDCODE
\& script cleverly gathers this material into 
the \c
.BEGINQUOTEDCODE
\&tags\c
.ENDQUOTEDCODE
\& file, 
which can be used as input to the next
formatting run.
As with LaTeX,  the cross-referencing information is always one
formatter run behind, and to get a consistent document you must keep
running \c
.BEGINQUOTEDCODE
\&noroff\c
.ENDQUOTEDCODE
\& until you reach a fixed point.
(\c
.BEGINQUOTEDCODE
\&noroff\c
.ENDQUOTEDCODE
\& ought to provide some help for this, to show when the tags
file has changed, for example.)
.HU "The shell wrapper"
Toroff is implemented as an awk program in a shell wrapper, and
uses a macro package to control the final formatting.  In the following
discussion, a sample macro package, suitable for use with either \c
.BEGINQUOTEDCODE
\&-mm\c
.ENDQUOTEDCODE
\&
or \c
.BEGINQUOTEDCODE
\&-ms\c
.ENDQUOTEDCODE
\&, is presented alongside the awk code which calls the
macros.  Users may wish to modify the sample macros to suit local
preferences.  Many do-nothing macros are provided as user exits so
knowledgeable users can modify document formatting without changing
the awk program.
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 1a "toroff" 1a \(==
.STARTXREF
.XREFNOTUSED

#!/bin/sh
AWK=nawk
\c
.USE "process arguments" 2a

\c
.USE "invoke awk program" 2b

.ENDCODECHUNK NWrof7-tor6-1
.BEGINDOCCHUNK
.P
Processing arguments is simple.
The only effect of the \c
.BEGINQUOTEDCODE
\&-delay\c
.ENDQUOTEDCODE
\& flag is to control placement of
the ``begin document'' document macro;
the noroff script supplies the command to source the noroff macros
as the very first line. This avoids groff warnings about undefined
strings in the generated markup for the indexing and cross-references.
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 2a "process arguments" 2a \(==
.STARTXREF
.XREFUSES
.ADDLIST 1a
.PRINTLIST

delay=0 noindex=0
for i do
    case $i in
        -delay)    delay=1   ;;
        -noindex)  noindex=1 ;;
        *) echo "This can't happen -- $i passed to toroff" 1>&2;
           exit 1;;
    esac
done
.ENDCODECHUNK NWrof7-proH-1
.BEGINDOCCHUNK
.P
Invoking the awk program is hard, because the program uses both
single quotes and double quotes, so neither can be used to protect the
other, and quoting each quote is ugly.  The pragmatic solution is to
copy the awk program into a temporary file, using a shell here-document.
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 2b "invoke awk program" 2b \(==
.STARTXREF
.XREFUSES
.ADDLIST 1a
.PRINTLIST

awkfile="/tmp/noweb$$.awk"
trap 'rm -f $awkfile' 0 1 2 10 14 15
cat > $awkfile \&<< 'EOF'
\c
.USE "awk program" 2c

EOF
$AWK -f $awkfile -v delay=$delay noindex=$noindex
.ENDCODECHUNK NWrof7-invI-1
.BEGINDOCCHUNK
.HU "The main program"
The overall structure of the awk program is simple.  The \c
.BEGINQUOTEDCODE
\&tags\c
.ENDQUOTEDCODE
\&
file is processed in the \c
.BEGINQUOTEDCODE
\&BEGIN\c
.ENDQUOTEDCODE
\& action, and the noweb source is
processed by awk's pattern-action loop.
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 2c "awk program" 2c \(==
.STARTXREF
.XREFDEFS
.ADDLIST 12b
.PRINTLIST
.XREFUSES
.ADDLIST 2b
.PRINTLIST

\c
.USE "read and process noweb source" 2d

\c
.USE "functions" 8a

.ENDCODECHUNK NWrof7-awkB-1
.BEGINDOCCHUNK
.HU "Beginning and ending the document"
The \c
.BEGINQUOTEDCODE
\&-delay\c
.ENDQUOTEDCODE
\& option allows the initial document chunk to be processed
before invoking the ``begin document'' macro,
so that necessary initialization may
be performed.  The only commands which are useful in such an initial chunk
are bare troff commands.  The delay is handled by special patterns for
the initial document chunk.  Because several noweb files can be
processed in the same formatting run, there can be several document
chunks numbered zero.  The later ones are not treated specially, by the
simple expedient of turning off \c
.BEGINQUOTEDCODE
\&delay\c
.ENDQUOTEDCODE
\& after the first one.  This
code also handles the \c
.BEGINQUOTEDCODE
\&trailer\c
.ENDQUOTEDCODE
\& at the end of the document.
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 2d "read and process noweb source" 2d \(==
.STARTXREF
.XREFDEFS
.ADDLIST 3b
.ADDLIST 3d
.ADDLIST 5c
.ADDLIST 5d
.ADDLIST 5e
.ADDLIST 6c
.ADDLIST 7a
.ADDLIST 8c
.ADDLIST 9a
.ADDLIST 9c
.ADDLIST 11a
.ADDLIST 11b
.PRINTLIST
.XREFUSES
.ADDLIST 2c
.PRINTLIST

/^@begin docs 0$/ { if (delay) next }
/^@end docs 0$/ { if (delay) { \c
.USE "start document" 2f
\&; delay = 0; next } }
/^@header m/ { if (!delay) { \c
.USE "start document" 2f
\& } }
/^@trailer/ { print ".ENDOFDOCUMENT" }
.ENDCODECHUNK NWrof7-reaT-1
.BEGINCODECHUNK
.DEFINITION 2e "tmac.w" 2e \(==
.STARTXREF
.XREFDEFS
.ADDLIST 3a
.ADDLIST 3c
.ADDLIST 3e
.ADDLIST 4a
.ADDLIST 4b
.ADDLIST 6a
.ADDLIST 6d
.ADDLIST 7b
.ADDLIST 8b
.ADDLIST 8d
.ADDLIST 9b
.ADDLIST 9d
.ADDLIST 10a
.ADDLIST 10c
.ADDLIST 11d
.ADDLIST 13b
.ADDLIST 14a
.PRINTLIST
.XREFNOTUSED

.de ENDOFDOCUMENT
..
.ENDCODECHUNK NWrof7-tma6-1
.BEGINCODECHUNK
.DEFINITION 2f "start document" 2f \(==
.STARTXREF
.XREFUSES
.ADDLIST 2d
.PRINTLIST

printf ".BEGINNINGOFDOCUMENT\n"
.ENDCODECHUNK NWrof7-staE-1
.BEGINCODECHUNK
.DEFINITION 3a "tmac.w" 2e \(pl\(==
.NEWLINE
\&.de BEGINNINGOFDOCUMENT\c
.NEWLINE
\&..\c
.NEWLINE
.ENDCODECHUNK NWrof7-tma6-2
.BEGINDOCCHUNK
.HU "Labels, references, and section numbers"
Here is the code to number code chunks in the page-number-and-letter
style used by noweb.  The current page number is always stored in
troff's \c
.BEGINQUOTEDCODE
\&%\c
.ENDQUOTEDCODE
\& number register, and we will arrange an
auto-incrementing number register \c
.BEGINQUOTEDCODE
\&SECTIONLETTER\c
.ENDQUOTEDCODE
\& which is
reset at each top-of-page and is formatted as lower-case alphabetic
characters using troff's \c
.BEGINQUOTEDCODE
\&.af\c
.ENDQUOTEDCODE
\& request.  The top-of-page trap is
sprung one unit below the top of the page in order not to interfere with
the top-of-page macros of \c
.BEGINQUOTEDCODE
\&-mm\c
.ENDQUOTEDCODE
\& or \c
.BEGINQUOTEDCODE
\&-ms\c
.ENDQUOTEDCODE
\&.
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 3b "read and process noweb source" 2d \(pl\(==
.NEWLINE
\&/^@xref label/ { lastxreflabel = $3 }\c
.NEWLINE
\&/^@xref ref/ { lastxrefref = tag(substr($0, 11)) }\c
.NEWLINE
.ENDCODECHUNK NWrof7-reaT-2
.BEGINCODECHUNK
.DEFINITION 3c "tmac.w" 2e \(pl\(==
.NEWLINE
\&.de RESETLETTER                     \e" reset letter at top of page\c
.NEWLINE
\&.nr SECTIONLETTER 0 1               \e" count code sections on same page\c
.NEWLINE
\&.af SECTIONLETTER a                 \e" ... formatted as lower-case alpha\c
.NEWLINE
\&..\c
.NEWLINE
\&.wh 1u RESETLETTER                  \e" trap just below top of page\c
.NEWLINE
.ENDCODECHUNK NWrof7-tma6-3
.BEGINDOCCHUNK
.P
Note that the link information on the previous chunk shows that this
chunk uses the \c
.BEGINQUOTEDCODE
\&code\c
.ENDQUOTEDCODE
\& variable, when in fact \c
.BEGINQUOTEDCODE
\&code\c
.ENDQUOTEDCODE
\& only appears
as text within a comment.  This error is an artifact of the language
independence of noweb.
.ENDDOCCHUNK
.BEGINDOCCHUNK
.HU "Beginning and ending chunks"
Except for the initial document chunk, which was processed above, all
beginnings and endings of documentation and code chunks are processed
here.  Variable \c
.BEGINQUOTEDCODE
\&code\c
.ENDQUOTEDCODE
\& is \c
.BEGINQUOTEDCODE
\&0\c
.ENDQUOTEDCODE
\& in text chunks, \c
.BEGINQUOTEDCODE
\&1\c
.ENDQUOTEDCODE
\& in code
chunks and \c
.BEGINQUOTEDCODE
\&2\c
.ENDQUOTEDCODE
\& in quoted code.  The argument to
the \c
.BEGINQUOTEDCODE
\&.ENDCODECHUNK\c
.ENDQUOTEDCODE
\& macro is the label which was in effect when the
code chunk was started, which is stored in the variable \c
.BEGINQUOTEDCODE
\&lastdefnlabel\c
.ENDQUOTEDCODE
\&.
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 3d "read and process noweb source" 2d \(pl\(==
.NEWLINE
\&/^@begin docs/ { printf ".BEGINDOCCHUNK\en" }\c
.NEWLINE
\&/^@end docs/ { printf ".ENDDOCCHUNK\en" }\c
.NEWLINE
\&/^@begin code/ { code = 1; printf ".BEGINCODECHUNK\en" }\c
.NEWLINE
\&/^@end code/ { code = 0; printf ".ENDCODECHUNK %s\en", lastdefnlabel }\c
.NEWLINE
.ENDCODECHUNK NWrof7-reaT-3
.BEGINDOCCHUNK
.P
The macros related to document chunks are simple:  \c
.BEGINQUOTEDCODE
\&.BEGINDOCCHUNK\c
.ENDQUOTEDCODE
\& does
nothing, and \c
.BEGINQUOTEDCODE
\&.ENDDOCCHUNK\c
.ENDQUOTEDCODE
\& merely flushes any unprinted output.
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 3e "tmac.w" 2e \(pl\(==
.NEWLINE
\&.de BEGINDOCCHUNK\c
.NEWLINE
\&..\c
.NEWLINE
\&.de ENDDOCCHUNK\c
.NEWLINE
\&.br\c
.NEWLINE
\&..\c
.NEWLINE
.ENDCODECHUNK NWrof7-tma6-4
.BEGINDOCCHUNK
.P
A code chunk uses a font, type size, leading, fill and adjust modes,
indent and tab stops which are distinct from those in the documentation.
These environment variables must be saved on entering a code chunk,
appropriate values for a code chunk must be established, and the
original values restored at the end of a code chunk.
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 4a "tmac.w" 2e \(pl\(==
.NEWLINE
\&.de BEGINCODECHUNK\c
.NEWLINE
\&.sp 0.4v\c
.NEWLINE
\c
.USE "save environment" 4c
\&\c
.NEWLINE
\c
.USE "set local environment" 4d
\&\c
.NEWLINE
\&.di CODECHUNK\c
.NEWLINE
\&..\c
.NEWLINE
\&.de ENDCODECHUNK\c
.NEWLINE
\c
.USE "read back code chunk from diversion" 5a
\&\c
.NEWLINE
\c
.USE "restore environment" 5b
\&\c
.NEWLINE
\&..\c
.NEWLINE
.ENDCODECHUNK NWrof7-tma6-5
.BEGINDOCCHUNK
.P
The code below replaces the built-in \c
.BEGINQUOTEDCODE
\&.ta\c
.ENDQUOTEDCODE
\& tab-setting command with a
custom tab-setting command that allows tab stops to be reset and later
recalled.
(A.R.: I don't see a use for this; furthermore this saves
the new settings in \c
.BEGINQUOTEDCODE
\&ta_saved\c
.ENDQUOTEDCODE
\&, it doesn't save the old ones.)
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 4b "tmac.w" 2e \(pl\(==
.NEWLINE
\&.rn ta real_ta\c
.NEWLINE
\&.de ta\c
.NEWLINE
\&.ds ta_saved \e\e$1 \e\e$2 \e\e$3 \e\e$4 \e\e$5 \e\e$6 \e\e$7 \e\e$8 \e\e$9\c
.NEWLINE
\&.real_ta \e\e$1 \e\e$2 \e\e$3 \e\e$4 \e\e$5 \e\e$6 \e\e$7 \e\e$8 \e\e$9\c
.NEWLINE
\&..\c
.NEWLINE
.ENDCODECHUNK NWrof7-tma6-6
.BEGINDOCCHUNK
.P
The font, type size and leading require two saved values, so
that both the current value and the previous value can be restored.
For those not intimately familiar with troff, \c
.BEGINQUOTEDCODE
\&.f\c
.ENDQUOTEDCODE
\& is the current
font number, \c
.BEGINQUOTEDCODE
\&.s\c
.ENDQUOTEDCODE
\& is the current point size, \c
.BEGINQUOTEDCODE
\&.v\c
.ENDQUOTEDCODE
\& is the current
vertical spacing, \c
.BEGINQUOTEDCODE
\&.u\c
.ENDQUOTEDCODE
\& is 1 or 0 for fill/no-fill, \c
.BEGINQUOTEDCODE
\&.j\c
.ENDQUOTEDCODE
\& is the
current adjust mode, and \c
.BEGINQUOTEDCODE
\&.i\c
.ENDQUOTEDCODE
\& is the current indent mode.
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 4c "save environment" 4c \(==
.STARTXREF
.XREFUSES
.ADDLIST 4a
.PRINTLIST

.nr OLDft \\n(.f
.ft P
.nr PREVft \\n(.f
.nr OLDps \\n(.s
.ps
.nr PREVps \\n(.s
.nr OLDvs \\n(.v
.vs
.nr PREVvs \\n(.v
.nr OLDfi \\n(.u
.nr OLDad \\n(.j
.nr OLDin \\n(.in
.ENDCODECHUNK NWrof7-savG-1
.BEGINDOCCHUNK
.P
Code is printed in a constant-width font at 80% the size of document text.
Code is collected in fill mode with explicit breaks after each line;
although it might seem more natural to collect lines of code in no-fill
mode, that is not possible here because uses of code chunks must be able
to appear on the same line as actual code.
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 4d "set local environment" 4d \(==
.STARTXREF
.XREFUSES
.ADDLIST 4a
.PRINTLIST

.ft CW
.ps \\n(.s*4/5
.vs \\n(.vu*4u/5u
.in +0.5i
.real_ta 0.5i 1i 1.5i 2i 2.5i 3i 3.5i 4i
.fi
.ENDCODECHUNK NWrof7-setL-1
.BEGINDOCCHUNK
.P
In order to prevent page breaks within code chunks, each code chunk
is read into a diversion and a page break is issued if the code chunk
is too big to fit on the current page.  After the diversion is read back
in, when it is known what page the code is printed on, the location of
the code chunk is written using a \c
.BEGINQUOTEDCODE
\&.tm\c
.ENDQUOTEDCODE
\& command.
(The \c
.BEGINQUOTEDCODE
\&dn\c
.ENDQUOTEDCODE
\& number register is the height of the most recent diversion,
and \c
.BEGINQUOTEDCODE
\&.t\c
.ENDQUOTEDCODE
\& is the distance to the next trap.)
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 5a "read back code chunk from diversion" 5a \(==
.STARTXREF
.XREFUSES
.ADDLIST 4a
.PRINTLIST

.br                        \" flush last line of code
.di                        \" end diversion
.if \\n(dn>\\n(.t .bp      \" force form feed if too big
.nf                        \" no fill mode -- already formatted
.in -0.5i                  \" don't re-indent when re-reading text
.CODECHUNK                 \" output body of diversion
.tm ###TAG### \\$1 \\n[%]\\n+[SECTIONLETTER] \" write tag info
.rm CODECHUNK              \" reset diversion for next code chunk
.ENDCODECHUNK NWrof7-reaZ-1
.BEGINDOCCHUNK
.P
Finally, here is the code to restore the environment that existed before
the beginning of the code chunk.
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 5b "restore environment" 5b \(==
.STARTXREF
.XREFUSES
.ADDLIST 4a
.PRINTLIST

.ft \\n[PREVft]
.ft \\n[OLDft]
.ps \\n[PREVps]
.ps \\n[OLDps]
.vs \\n[PREVvs]u
.vs \\n[OLDvs]u
.if \\n[OLDfi] .fi
.ad \\n[OLDad]
.in \\n[OLDin]u
.real_ta
.ENDCODECHUNK NWrof7-resJ-1
.BEGINDOCCHUNK
.HU "Text, code, and quoted code"
In text chunks, text is copied from input to output basically unchanged.
However, when quoted code is included in a line of text, the quoted code
is processed by separate macros and it is possible that the continuation
of the line of text after the quoted code begins with a character which
is special to troff at the beginning of a line:  a space, period, or
single quote.  To prevent problems in this case, variable \c
.BEGINQUOTEDCODE
\&text\c
.ENDQUOTEDCODE
\& is
set to \c
.BEGINQUOTEDCODE
\&0\c
.ENDQUOTEDCODE
\& at the beginning of each line and incremented whenever text
is written, and each text after the first is guarded by a non-printing
null character.
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 5c "read and process noweb source" 2d \(pl\(==
.NEWLINE
\&/^@text/ && code == 0 { s = substr($0, 7)\c
.NEWLINE
\&                        if (text++) printf "\e\e&"\c
.NEWLINE
\&                        printf "%s", substr($0, 7) }\c
.NEWLINE
\&/^@nl/ && code != 1 { text = 0; printf "\en" }\c
.NEWLINE
.ENDCODECHUNK NWrof7-reaT-4
.BEGINDOCCHUNK
.P
In code chunks, backslashes are replaced with troff's \c
.BEGINQUOTEDCODE
\&\ee\c
.ENDQUOTEDCODE
\& escape
sequence and a non-printing null character is added at the beginning of
each text to guard leading periods.  If your macro package issues a \c
.BEGINQUOTEDCODE
\&.ec\c
.ENDQUOTEDCODE
\&
command, this code will have to change; fortunately, both \c
.BEGINQUOTEDCODE
\&-mm\c
.ENDQUOTEDCODE
\& and
\c
.BEGINQUOTEDCODE
\&-ms\c
.ENDQUOTEDCODE
\& are nicely-behaved with respect to escape characters.
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 5d "read and process noweb source" 2d \(pl\(==
.NEWLINE
\&/^@text/ && code != 0 { s = substr($0, 7)\c
.NEWLINE
\&                        gsub(/\e\e/, "\e\ee", s)\c
.NEWLINE
\&                        printf "\e\e&%s\e\ec\en", s }\c
.NEWLINE
.ENDCODECHUNK NWrof7-reaT-5
.BEGINDOCCHUNK
.P
Dealing with newlines in code chunks is surprisingly hard.  We would like
to print lines of code in \c
.BEGINQUOTEDCODE
\&nofill\c
.ENDQUOTEDCODE
\& mode, but since noweb's markup
and filter programs may split lines of code, we are forced to use
\c
.BEGINQUOTEDCODE
\&fill\c
.ENDQUOTEDCODE
\& mode and deal with newlines ourselves.  Further, if a line is
too long to print on a single line, it must be split, and the continuation
line right-justified instead of left-justified.  All of this processing is
accomplished by calling a macro at each newline; the macro plants a
page-position trap at the next line so that any continuation line will be
right-justified.
(The \c
.BEGINQUOTEDCODE
\&.dt\c
.ENDQUOTEDCODE
\& command defines a trap for use within the current
diversion, whereas the more common \c
.BEGINQUOTEDCODE
\&.wh\c
.ENDQUOTEDCODE
\& is for a particular
page position.)
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 5e "read and process noweb source" 2d \(pl\(==
.NEWLINE
\&/^@nl/ && code == 1 { printf ".NEWLINE\en" }\c
.NEWLINE
.ENDCODECHUNK NWrof7-reaT-6
.BEGINCODECHUNK
.DEFINITION 6a "tmac.w" 2e \(pl\(==
.NEWLINE
\&.de LINETRAP\c
.NEWLINE
\&.dt \e\en[TRAPplace]u    \e"cancel current trap\c
.NEWLINE
\&'ad r                  \e" right-adjust continuation lines\c
.NEWLINE
\&..\c
.NEWLINE
\&.de NEWLINE\c
.NEWLINE
\&.dt \e\en[TRAPplace]u    \e" cancel current trap\c
.NEWLINE
\&\e&                     \e" end continued word\c
.NEWLINE
\&.br                    \e" flush output\c
.NEWLINE
\c
.USE "plant newline trap" 6b
\&\c
.NEWLINE
\&..\c
.NEWLINE
.ENDCODECHUNK NWrof7-tma6-7
.BEGINDOCCHUNK
.P
The newline trap is planted after each definition line, for the first
line of code in the code chunk, and again after each newline in the
code chunk.
(The \c
.BEGINQUOTEDCODE
\&.d\c
.ENDQUOTEDCODE
\& register indicates the current vertical position
within the diversion.)
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 6b "plant newline trap" 6b \(==
.STARTXREF
.XREFUSES
.ADDLIST 6a
.ADDLIST 7b
.PRINTLIST

.nr TRAPplace \\n(.du+1u       \" location of next trap
.dt \\n[TRAPplace]u LINETRAP   \" plant trap at next line
.ad l                          \" left-adjust first line
.ENDCODECHUNK NWrof7-plaI-1
.BEGINDOCCHUNK
.P
Quoted code in a documentation chunk is printed within its own macros,
which must not cause a break.
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 6c "read and process noweb source" 2d \(pl\(==
.NEWLINE
\&/^@quote/ { code = 2; printf "\e\ec\en.BEGINQUOTEDCODE\en" }\c
.NEWLINE
\&/^@endquote/ { code = 0; text++; printf ".ENDQUOTEDCODE\en" }\c
.NEWLINE
.ENDCODECHUNK NWrof7-reaT-7
.BEGINCODECHUNK
.DEFINITION 6d "tmac.w" 2e \(pl\(==
.NEWLINE
\&.de BEGINQUOTEDCODE\c
.NEWLINE
\&.nr SAVEft \e\en(.f\c
.NEWLINE
\&.ft P\c
.NEWLINE
\&.nr SAVEftP \e\en(.f\c
.NEWLINE
\&.ft CW\c
.NEWLINE
\&..\c
.NEWLINE
\&.de ENDQUOTEDCODE\c
.NEWLINE
\&.ft \e\en[SAVEftP]\c
.NEWLINE
\&.ft \e\en[SAVEft]\c
.NEWLINE
\&..\c
.NEWLINE
.ENDCODECHUNK NWrof7-tma6-8
.BEGINDOCCHUNK
.HU "Definitions and uses of code chunks"
Definitions and uses of code chunks are handled below.  Variable
\c
.BEGINQUOTEDCODE
\&defn[name]\c
.ENDQUOTEDCODE
\& is set to a plus sign after a definition is printed, so
that continuations of the definition are properly identified.  Variable
\c
.BEGINQUOTEDCODE
\&lastxrefref\c
.ENDQUOTEDCODE
\& is the tag associated with the most-recently-seen
cross-reference label, and refers to the section number of the original
definition of the code chunk.
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 7a "read and process noweb source" 2d \(pl\(==
.NEWLINE
\&/^@defn/ { name = convquote(substr($0, 7))\c
.NEWLINE
\&           lastdefnlabel = lastxreflabel\c
.NEWLINE
\&           if (! (name in defn))\c
.NEWLINE
\&                defn[name] = "\e\e(=="\c
.NEWLINE
\&           printf ".DEFINITION %s \e"%s\e" %s %s\en",\c
.NEWLINE
\&                  tag(lastdefnlabel), name, lastxrefref, defn[name]\c
.NEWLINE
\&           defn[name] = "\e\e(pl\e\e(==" }\c
.NEWLINE
\&/^@use/ { name = convquote(substr($0, 6))\c
.NEWLINE
\&          printf "\e\ec\en"\c
.NEWLINE
\&          printf ".USE \e"%s\e" %s\en", name, lastxrefref }\c
.NEWLINE
.ENDCODECHUNK NWrof7-reaT-8
.BEGINDOCCHUNK
The original version of toroff used \c
.BEGINQUOTEDCODE
\&\e(oG\c
.ENDQUOTEDCODE
\& and \c
.BEGINQUOTEDCODE
\&\e(cG\c
.ENDQUOTEDCODE
\& for
the double angle bracket characters.  These appear to be non-standard,
thus we define our own strings to take their place. (A.R.)
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 7b "tmac.w" 2e \(pl\(==
.NEWLINE
\&.ds o< <\eh'-0.1m'<\c
.NEWLINE
\&.ds c> >\eh'-0.1m'>\c
.NEWLINE
\&.de DEFINITION\c
.NEWLINE
\&.ti -0.5i\c
.NEWLINE
\&\e\efR\e\e(sc\e\e$1   \e\e*(o<\e\e$2 \e\e$3\e\e*(c>\e\e$4\e\efP\c
.NEWLINE
\c
.USE "plant newline trap" 6b
\&\c
.NEWLINE
\&..\c
.NEWLINE
\&.de USE\c
.NEWLINE
\&\e\efR\e\e*(o<\e\e$1 \e\e$2\e\e*(c>\e\efP\ec   \e" section name and original number\c
.NEWLINE
\&..\c
.NEWLINE
.ENDCODECHUNK NWrof7-tma6-9
.BEGINDOCCHUNK
.P
Processing of quoted code within definitions and uses is performed by the
\c
.BEGINQUOTEDCODE
\&convquote\c
.ENDQUOTEDCODE
\& function.  This must be processed differently than quoted
code within text because the noweb markup program doesn't emit
\c
.BEGINQUOTEDCODE
\&@quote\c
.ENDQUOTEDCODE
\& .\|.\|. \c
.BEGINQUOTEDCODE
\&@endquote\c
.ENDQUOTEDCODE
\& markers within definitions and uses.  And,
because the subject string is used as an argument to a macro, it would not
possible to call the \c
.BEGINQUOTEDCODE
\&.BEGINQUOTEDCODE\c
.ENDQUOTEDCODE
\& and \c
.BEGINQUOTEDCODE
\&.ENDQUOTEDCODE\c
.ENDQUOTEDCODE
\& macros,
even if noweb did emit the markers within definitions and uses.
.P
The original version just did \c
.BEGINQUOTEDCODE
\&gsub(/\e]\e]/, "\e\e*[ENDCONVQUOTE]", s)\c
.ENDQUOTEDCODE
\&,
but this does not catch the case of something like \f(CW[\&[a[i\&]\&]\&]\fP.
A more complicated loop using \c
.BEGINQUOTEDCODE
\&match()\c
.ENDQUOTEDCODE
\& is in order.
It loops through the string, picking it apart, and replacing
each quoted code section individually.
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 8a "functions" 8a \(==
.STARTXREF
.XREFDEFS
.ADDLIST 12a
.PRINTLIST
.XREFUSES
.ADDLIST 2c
.ADDLIST 13a
.PRINTLIST

function convquote(s,   out, front, mid, tail) {
    gsub(/\[\[/, "\\*[BEGINCONVQUOTE]", s)
    # gsub(/\]\]/, "\\*[ENDCONVQUOTE]", s)
    out = ""
    mid = "\\*[ENDCONVQUOTE]"
    while (match(s, /\]\]+/) != 0) {
        # RLENGTH is length of match, want to remove last two chars
        # RSTART is where sequence of ]s begins
        tail = substr(s, RSTART + RLENGTH)
        if (RLENGTH == 2) # easy
                front = substr(s, 1, RSTART - 1)
        else
                front = substr(s, 1, RSTART - 1 + RLENGTH - 2)
        out = out front mid
        s = tail
    }
    out = out s
    return out }
# my test program for the revised function - ADR
# BEGIN { str = "abc[[foo[i]]]]]]]]junk"
#       print str
#       print convquote(str)
#       str2 = "nothing here"
#       print str2
#       print convquote(str2)
#       str3 = "abc[[foo[i]]]]]]]]junk[[bar[i]]more stuff[[baz]]"
#       print str3
#       print convquote(str3)
# }
.ENDCODECHUNK NWrof7-fun9-1
.BEGINCODECHUNK
.DEFINITION 8b "tmac.w" 2e \(pl\(==
.NEWLINE
\&.ds BEGINCONVQUOTE \ef[CW]\c
.NEWLINE
\&.ds ENDCONVQUOTE   \efP\c
.NEWLINE
.ENDCODECHUNK NWrof7-tma6-A
.BEGINDOCCHUNK
.HU "Cross-referencing and indexing"
The code for producing the ``hypertext links'' at the end of each
code section is given below: for each type, the beginning of the message
is printed, items are accumulated in a list, and the list is printed
after the number of items in the list is known.  The first time any of
the cross-reference or identifier-index messages appears, it is necessary
to reset the point size and leading to the small font used for this
material, which is 80% of the size of code and 64% of the size of text.
All of these lines are part of the diversion which collects a
code chunk.  First is the code to report definitions and uses of code.
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 8c "read and process noweb source" 2d \(pl\(==
.NEWLINE
\&/^@xref begindefs/     { \c
\c
.USE "checkcode" 10b
\&; printf ".XREFDEFS\en" }\c
.NEWLINE
\&/^@xref beginuses/     { \c
\c
.USE "checkcode" 10b
\&; printf ".XREFUSES\en" }\c
.NEWLINE
\&/^@xref notused/       { \c
\c
.USE "checkcode" 10b
\&; printf ".XREFNOTUSED\en" }\c
.NEWLINE
\&/^@xref (def|use)item/ { printf ".ADDLIST %s\en", tag($3) }\c
.NEWLINE
\&/^@xref end(def|use)s/ { printf ".PRINTLIST\en" }\c
.NEWLINE
.ENDCODECHUNK NWrof7-reaT-9
.BEGINCODECHUNK
.DEFINITION 8d "tmac.w" 2e \(pl\(==
.NEWLINE
\&.de XREFDEFS\c
.NEWLINE
\&This definition continued in\c
.NEWLINE
\&..\c
.NEWLINE
\&.de XREFUSES\c
.NEWLINE
\&This code used in\c
.NEWLINE
\&..\c
.NEWLINE
\&.de XREFNOTUSED\c
.NEWLINE
\&This code is not used in this document.\c
.NEWLINE
\&.br\c
.NEWLINE
\&..\c
.NEWLINE
.ENDCODECHUNK NWrof7-tma6-B
.BEGINDOCCHUNK
.P
The code to report the definitions of identifiers appears below.  The
\c
.BEGINQUOTEDCODE
\&if\c
.ENDQUOTEDCODE
\& in \c
.BEGINQUOTEDCODE
\&@index isused\c
.ENDQUOTEDCODE
\& prevents index definitions from pointing
to themselves.
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 9a "read and process noweb source" 2d \(pl\(==
.NEWLINE
\&/^@index begindefs/ && !noindex { \c
\c
.USE "checkcode" 10b
\&; printf ".INDEXDEF\en" }\c
.NEWLINE
\&/^@index isused/ && !noindex {\c
.NEWLINE
\&    if (tag($3) != lastxrefref) printf ".ADDLIST %s\en", tag($3) }\c
.NEWLINE
\&/^@index defitem/ && !noindex { printf ".DEFITEM %s\en.PRINTLIST\en", $3 }\c
.NEWLINE
.ENDCODECHUNK NWrof7-reaT-A
.BEGINCODECHUNK
.DEFINITION 9b "tmac.w" 2e \(pl\(==
.NEWLINE
\&.de INDEXDEF\c
.NEWLINE
\&Defines:\c
.NEWLINE
\&.br\c
.NEWLINE
\&..\c
.NEWLINE
\&.de DEFITEM\c
.NEWLINE
\&.ti +1m\c
.NEWLINE
\&\e\e*[BEGINCONVQUOTE]\e\e$1\e\e*[ENDCONVQUOTE],\c
.NEWLINE
\&.if \e\en[NLIST]>0 used in\c
.NEWLINE
\&..\c
.NEWLINE
.ENDCODECHUNK NWrof7-tma6-C
.BEGINDOCCHUNK
.P
Finally, here is the code to report uses of identifiers.
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 9c "read and process noweb source" 2d \(pl\(==
.NEWLINE
\&/^@index beginuses/ && !noindex { \c
\c
.USE "checkcode" 10b
\&; printf ".INDEXUSE\en" }\c
.NEWLINE
\&/^@index isdefined/ && !noindex { lastuse = tag($3) }\c
.NEWLINE
\&/^@index useitem/   && !noindex {\c
.NEWLINE
\&    printf ".ADDLIST \e"\e\e*[BEGINCONVQUOTE]%s\e\e*[ENDCONVQUOTE] %s\e"\en",\c
.NEWLINE
\&            $3, lastuse }\c
.NEWLINE
\&/^@index enduses/   && !noindex { printf ".PRINTLIST\en" }\c
.NEWLINE
.ENDCODECHUNK NWrof7-reaT-B
.BEGINCODECHUNK
.DEFINITION 9d "tmac.w" 2e \(pl\(==
.NEWLINE
\&.de INDEXUSE\c
.NEWLINE
\&Uses\c
.NEWLINE
\&..\c
.NEWLINE
.ENDCODECHUNK NWrof7-tma6-D
.BEGINDOCCHUNK
.P
The macros \c
.BEGINQUOTEDCODE
\&.ADDLIST s\c
.ENDQUOTEDCODE
\&, which adds string \c
.BEGINQUOTEDCODE
\&s\c
.ENDQUOTEDCODE
\& to a queued list
waiting to be printed, and \c
.BEGINQUOTEDCODE
\&.PRINTLIST\c
.ENDQUOTEDCODE
\&, which prints the list,
appropriately formatted with commas, are described below.  These macros
make use of some interesting troff idioms.  \c
.BEGINQUOTEDCODE
\&LIST\c
.ENDQUOTEDCODE
\& is an array of
strings; the \c
.BEGINQUOTEDCODE
\&n\c
.ENDQUOTEDCODE
\&-th string in \c
.BEGINQUOTEDCODE
\&LIST\c
.ENDQUOTEDCODE
\& can be set to \c
.BEGINQUOTEDCODE
\&s\c
.ENDQUOTEDCODE
\& by 
\c
.BEGINQUOTEDCODE
\&.ds LISTn s\c
.ENDQUOTEDCODE
\&.  The expression \c
.BEGINQUOTEDCODE
\&.nr n \e\e$1\c
.ENDQUOTEDCODE
\& converts the string
passed as the first argument to a macro to the number \c
.BEGINQUOTEDCODE
\&n\c
.ENDQUOTEDCODE
\&.  Loops
are implemented in troff as recursive macros, as in \c
.BEGINQUOTEDCODE
\&.PRINTITEM\c
.ENDQUOTEDCODE
\&.
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 10a "tmac.w" 2e \(pl\(==
.NEWLINE
\&.nr NLIST 0 1 \e" initialize list index to 0 with auto-increment 1\c
.NEWLINE
\&.de ADDLIST\c
.NEWLINE
\&.ds LIST\e\en+[NLIST] \e\e$1\c
.NEWLINE
\&..\c
.NEWLINE
\&.de PRINTITEM\c
.NEWLINE
\&.nr PLIST \e\e$1\c
.NEWLINE
\&.nr PLISTPLUS \e\en[PLIST]+1\c
.NEWLINE
\&.if \e\en[NLIST]-\e\en[PLIST]<0 not used in this document.\c
.NEWLINE
\&.if \e\en[NLIST]-\e\en[PLIST]=0 \e\e*[LIST\e\en[PLIST]].\c
.NEWLINE
\&.if \e\en[NLIST]-\e\en[PLIST]=1 \e\c
.NEWLINE
\&        \e\e*[LIST\e\en[PLIST]] and \e\e*[LIST\e\en[PLISTPLUS]].\c
.NEWLINE
\&.if \e\en[NLIST]-\e\en[PLIST]>1 \e{ \e\e*[LIST\e\en[PLIST]],\c
.NEWLINE
\&.                        PRINTITEM 1+\e\en[PLIST] \e}\c
.NEWLINE
\&..\c
.NEWLINE
\&.de PRINTLIST\c
.NEWLINE
\&.PRINTITEM 1\c
.NEWLINE
\&.br\c
.NEWLINE
\&.nr NLIST 0 1 \e" re-initialize for next list\c
.NEWLINE
\&..\c
.NEWLINE
.ENDCODECHUNK NWrof7-tma6-E
.BEGINDOCCHUNK
.P
This code and macro reduces the font size in which the cross-reference
and identifier index information is printed at the end of a code chunk.
The \c
.BEGINQUOTEDCODE
\&code\c
.ENDQUOTEDCODE
\& variable is set to \c
.BEGINQUOTEDCODE
\&0\c
.ENDQUOTEDCODE
\& to ensure that \c
.BEGINQUOTEDCODE
\&.STARTXREF\c
.ENDQUOTEDCODE
\&
is performed only once at the end of each code chunk.  This code also
resets the adjustment mode, which was changed to left-adjustment or
right-adjustment by the \c
.BEGINQUOTEDCODE
\&LINETRAP\c
.ENDQUOTEDCODE
\& macro, and cancels the \c
.BEGINQUOTEDCODE
\&NEWLINE\c
.ENDQUOTEDCODE
\&
trap at \c
.BEGINQUOTEDCODE
\&TRAPplace\c
.ENDQUOTEDCODE
\&.
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 10b "checkcode" 10b \(==
.STARTXREF
.XREFUSES
.ADDLIST 8c
.ADDLIST 9a
.ADDLIST 9c
.PRINTLIST

if (code) { code = 0; printf ".STARTXREF\n" }
.ENDCODECHUNK NWrof7-che9-1
.BEGINCODECHUNK
.DEFINITION 10c "tmac.w" 2e \(pl\(==
.NEWLINE
\&.de STARTXREF\c
.NEWLINE
\&.ps \e\en(.s*4/5\c
.NEWLINE
\&.vs \e\en(.vu*4u/5u\c
.NEWLINE
\&.ft \e\en[OLDft]\c
.NEWLINE
\&.ad \e\en[OLDad]\c
.NEWLINE
\&.dt \e\en[TRAPplace]u\c
.NEWLINE
\&.sp 0.4v\c
.NEWLINE
\&..\c
.NEWLINE
.ENDCODECHUNK NWrof7-tma6-F
.BEGINDOCCHUNK
.HU "Collecting the lists of chunks and identifiers"
Collecting the lists of chunks and identifiers takes two passes over the
file, because the section numbers which tags refer to aren't known on
the first pass.  Therefore, the strategy on the first pass is to write
the chunk and identifier index entries to the \c
.BEGINQUOTEDCODE
\&tags\c
.ENDQUOTEDCODE
\& file on standard
error, and actually prepare the lists when reading the \c
.BEGINQUOTEDCODE
\&tags\c
.ENDQUOTEDCODE
\& file on
the second pass.  Thus, the first pass code shown here merely copies
the chunk and identifier index entries from the noweb intermediate
file to the \c
.BEGINQUOTEDCODE
\&tags\c
.ENDQUOTEDCODE
\& file using troff's \c
.BEGINQUOTEDCODE
\&.tm\c
.ENDQUOTEDCODE
\& command.  The
first section below handles chunks, the second section handles
identifiers.
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 11a "read and process noweb source" 2d \(pl\(==
.NEWLINE
\&/^@xref beginchunks/ { printf ".tm ###BEGINCHUNKS###\en" }\c
.NEWLINE
\&/^@xref chunkbegin/  { printf ".tm ###CHUNKBEGIN### %s\en",\c
.NEWLINE
\&                           substr($0, length($3) + 19) }\c
.NEWLINE
\&/^@xref chunkuse/    { printf ".tm ###CHUNKUSE### %s\en", $3 }\c
.NEWLINE
\&/^@xref chunkdefn/   { printf ".tm ###CHUNKDEFN### %s\en", $3 }\c
.NEWLINE
\&/^@xref chunkend/    { printf ".tm ###CHUNKEND###\en" }\c
.NEWLINE
\&/^@xref endchunks/   { printf ".tm ###ENDCHUNKS###\en" }\c
.NEWLINE
.ENDCODECHUNK NWrof7-reaT-C
.BEGINCODECHUNK
.DEFINITION 11b "read and process noweb source" 2d \(pl\(==
.NEWLINE
\&/^@index beginindex/ { printf ".tm ###BEGININDEX###\en" }\c
.NEWLINE
\&/^@index entrybegin/ { printf ".tm ###ENTRYBEGIN### %s\en",\c
.NEWLINE
\&                           substr($0, length($3) + 20) }\c
.NEWLINE
\&/^@index entryuse/   { printf ".tm ###ENTRYUSE### %s\en", $3 }\c
.NEWLINE
\&/^@index entrydefn/  { printf ".tm ###ENTRYDEFN### %s\en", $3 }\c
.NEWLINE
\&/^@index entryend/   { printf ".tm ###ENTRYEND###\en" }\c
.NEWLINE
\&/^@index endindex/   { printf ".tm ###ENDINDEX###\en" }\c
.NEWLINE
.ENDCODECHUNK NWrof7-reaT-D
.BEGINDOCCHUNK
.HU "The \f[CW]tags\fP file"
The \c
.BEGINQUOTEDCODE
\&tags\c
.ENDQUOTEDCODE
\& file is re-created at each formatter run by the troff
idiom (some people would call it a trick) of capturing the output of
troff's \c
.BEGINQUOTEDCODE
\&.tm\c
.ENDQUOTEDCODE
\& command, which writes to the standard error, in a
file via command-line redirection.  The code below uses an awk idiom; the
\c
.BEGINQUOTEDCODE
\&sub\c
.ENDQUOTEDCODE
\& simultaneously tests for a match and deletes the matched text.
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 11c "action for \*[BEGINCONVQUOTE]tags\*[ENDCONVQUOTE] line" 11c \(==
.STARTXREF
.XREFUSES
.ADDLIST 12b
.ADDLIST 13a
.PRINTLIST

{
    if      (sub(/^###TAG### /       , "")) tags[$1] = $2
    else if (sub(/^###BEGINCHUNKS###/, "")) printf ".de CLIST\n.CLISTBEGIN\n"
    else if (sub(/^###CHUNKBEGIN### /, "")) { name = convquote($0)
                                              chunkuse = chunkdefn = "" }
    else if (sub(/^###CHUNKUSE### /  , "")) chunkuse = chunkuse " " tag($0)
    else if (sub(/^###CHUNKDEFN### / , "")) chunkdefn = chunkdefn " " tag($0)
    else if (sub(/^###CHUNKEND###/   , ""))
       printf ".CITEM \"%s\" \"%s\" \"%s\"\n", name, chunkdefn, chunkuse
    else if (sub(/^###ENDCHUNKS###/  , "")) printf ".CLISTEND\n..\n"
    else if (sub(/^###BEGININDEX###/ , "")) printf ".de ILIST\n.ILISTBEGIN\n"
    else if (sub(/^###ENTRYBEGIN### /, "")) { name = convquote($0)
                                              entryuse = entrydefn = "" }
    else if (sub(/^###ENTRYUSE### /  , "")) entryuse = entryuse " " tag($0)
    else if (sub(/^###ENTRYDEFN### / , "")) entrydefn = entrydefn " " tag($0)
    else if (sub(/^###ENTRYEND###/   , "")) {
        for (i = 1; i <= split(entrydefn, entryarray); i++)
            sub(entryarray[i], "\\*[BEGINDEFN]&\\*[ENDDEFN]", entryuse)
        printf ".IITEM \"%s\" \"%s\"\n", name, entryuse }
    else if (sub(/^###ENDINDEX###/   , "")) printf ".ILISTEND\n..\n" 
}
.ENDCODECHUNK NWrof7-actO-1
.BEGINDOCCHUNK
.P
The \c
.BEGINQUOTEDCODE
\&sub\c
.ENDQUOTEDCODE
\& within the \c
.BEGINQUOTEDCODE
\&ENTRYEND\c
.ENDQUOTEDCODE
\& causes definitions of identifiers
to be italicized, according to the following defined strings.
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 11d "tmac.w" 2e \(pl\(==
.NEWLINE
\&.ds BEGINDEFN  \efI\c
.NEWLINE
\&.ds ENDDEFN    \efP\c
.NEWLINE
.ENDCODECHUNK NWrof7-tma6-G
.BEGINDOCCHUNK
.P
Lookup of the section number corresponding to a particular tag is
performed by the \c
.BEGINQUOTEDCODE
\&tag\c
.ENDQUOTEDCODE
\& function.
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 12a "functions" 8a \(pl\(==
.NEWLINE
\&function tag(s) { if (s in tags) return tags[s]; else return "???" }\c
.NEWLINE
.ENDCODECHUNK NWrof7-fun9-2
.BEGINDOCCHUNK
.P
To use the tags, we have to read the tags file from the back end.
The tags file name is the basename of the file, with the suffix
replaced with \c
.BEGINQUOTEDCODE
\&".nwt"\c
.ENDQUOTEDCODE
\& (see the toroff script, below).
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 12b "awk program" 2c \(pl\(==
.NEWLINE
\&/^@file / {\c
.NEWLINE
\&  if (tagsfile == "") {\c
.NEWLINE
\&    tagsfile = substr($0, 7)\c
.NEWLINE
\&    sub(".*/", "", tagsfile)\c
.NEWLINE
\&    sub(/\e.[^.]*$/, "", tagsfile)\c
.NEWLINE
\&    tagsfile = tagsfile ".nwt"\c
.NEWLINE
\&    while (getline <tagsfile > 0) \c
\c
.USE "action for \*[BEGINCONVQUOTE]tags\*[ENDCONVQUOTE] line" 11c
\&\c
.NEWLINE
\&  }\c
.NEWLINE
\&}\c
.NEWLINE
.ENDCODECHUNK NWrof7-awkB-2
.BEGINDOCCHUNK

.HU "Using \\f[CW]toroff\fP"
Toroff is one element of the normal noweave pipeline.  Using
toroff in its full generality is hard.  The sample program shown
below 
allows the user to specify the macro package (although it defaults to
\c
.BEGINQUOTEDCODE
\&-mgm\c
.ENDQUOTEDCODE
\&),
and allows the user to specify troff pre-processors,
post-processors and options.
(Command line options to \c
.BEGINQUOTEDCODE
\&groff\c
.ENDQUOTEDCODE
\& can be used to invoke tbl, pic, etc.)
See the section below on how to generate the final formatted document.
.P
To avoid a race condition reading the tags file
(troff is writing to it while the awk program is reading it),
we first copy it for
use by the awk program, and then remove the copy.
The location of \c
.BEGINQUOTEDCODE
\&macrodir\c
.ENDQUOTEDCODE
\& is likely to require customization.
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 13a "noroff" 13a \(==
.STARTXREF
.XREFNOTUSED

#!/bin/sh
#
# noroff -- run troff using tags file trick

ROFF="groff"
AWK=nawk
macrodir=|LIBDIR|

opts=

if [ $# -eq 0 ]; then
  echo "Usage: noroff [groff-arguments] files" 1>&2
  exit 1
fi

while [ $# -gt 0 ]
do
        case $1 in
        -*)     opts="$opts $1"
                shift
                ;;
        *)      # end of options
                break;
                ;;
        esac
done

if [ "$opts" = "" ]
then
        # no options, default to -mgm
        # groff already defaults to -Tps
        opts="-mgm"
fi
# otherwise assume user passed in all the arguments they want

base="`basename $1 | sed '/\./s/\.[^.]*$//'`"
tagsfile="$base.nwt"
(echo ".so $macrodir/tmac.w"
if [ -r "$tagsfile" ]; then 
   cp $tagsfile /tmp/tags.$$
   $AWK '\c
.USE "action for \*[BEGINCONVQUOTE]tags\*[ENDCONVQUOTE] line" 11c
\&
         \c
.USE "functions" 8a
\&' /tmp/tags.$$
   rm -f /tmp/tags.$$
 fi
 cat "$@") |
($ROFF $opts 2>$tagsfile)
sed '/^###[A-Z][A-Z]*###/d' $tagsfile >&2
.ENDCODECHUNK NWrof7-nor6-1
.BEGINDOCCHUNK
.HU "Macros for indexing chunks and identifiers"
Toroff creates macros \c
.BEGINQUOTEDCODE
\&.CLIST\c
.ENDQUOTEDCODE
\& and \c
.BEGINQUOTEDCODE
\&.ILIST\c
.ENDQUOTEDCODE
\& which insert the
lists of chunks and identifiers, respectively, in the output.  These
macros, in turn, call other macros which format the lists.  The macros
below cause each item to be written on a separate line, with continuation
lines indented one-quarter inch.
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 13b "tmac.w" 2e \(pl\(==
.NEWLINE
\&.de CLISTBEGIN\c
.NEWLINE
\&.in +0.25i\c
.NEWLINE
\&..\c
.NEWLINE
\&.de CITEM\c
.NEWLINE
\&.ti -0.25i\c
.NEWLINE
\&.ie '\e\e$3'' \e\e*(o<\e\e$1 \e\e$2\e\e*(c> Not used in this document.\c
.NEWLINE
\&.el         \e\e*(o<\e\e$1 \e\e$2\e\e*(c> \e\e$3\c
.NEWLINE
\&.br\c
.NEWLINE
\&..\c
.NEWLINE
\&.de CLISTEND\c
.NEWLINE
\&.in -0.25i\c
.NEWLINE
\&..\c
.NEWLINE
.ENDCODECHUNK NWrof7-tma6-H
.BEGINCODECHUNK
.DEFINITION 14a "tmac.w" 2e \(pl\(==
.NEWLINE
\&.de ILISTBEGIN\c
.NEWLINE
\&.in +0.25i\c
.NEWLINE
\&..\c
.NEWLINE
\&.de IITEM\c
.NEWLINE
\&.ti -0.25i\c
.NEWLINE
\&\e\ef[CW]\e\e$1\e\efP: \e\e$2\c
.NEWLINE
\&.br\c
.NEWLINE
\&..\c
.NEWLINE
\&.de ILISTEND\c
.NEWLINE
\&.in -0.25i\c
.NEWLINE
\&..\c
.NEWLINE
.ENDCODECHUNK NWrof7-tma6-I
.BEGINDOCCHUNK
.HU "Using \f[CW]noweave -troff\fP and \f[CW]noroff\fP"
To produce the final document,
the tags file is needed for both the \c
.BEGINQUOTEDCODE
\&noweave\c
.ENDQUOTEDCODE
\&
and the \c
.BEGINQUOTEDCODE
\&noroff\c
.ENDQUOTEDCODE
\& steps.  This creates a bit of a boot-strapping
problem, since it is troff that first creates the tags file.
The way to do it is to run \c
.BEGINQUOTEDCODE
\&noweave\c
.ENDQUOTEDCODE
\& and \c
.BEGINQUOTEDCODE
\&noroff\c
.ENDQUOTEDCODE
\& in turn, twice.
For example, to make a printable version of this document, you would
run the programs like this.
.ENDDOCCHUNK
.BEGINCODECHUNK
.DEFINITION 14b "Example run" 14b \(==
.STARTXREF
.XREFNOTUSED

noweave -index -troff roff.nw > roff.tr
noroff roff.tr > /dev/null      # ignore warnings about CLIST and ILIST
noweave -index -troff roff.nw > roff.tr
noroff roff.tr > roff.ps        # print this one
.ENDCODECHUNK NWrof7-ExaB-1
.BEGINDOCCHUNK
.HU "Index of chunks and identifiers"
The automatically-generated index of chunks and identifiers for
toroff is shown below.
.S 10 12
.sp
.CLIST
.sp
.2C
.ILIST
.ENDDOCCHUNK


.tm ###BEGINCHUNKS###
.tm ###CHUNKBEGIN### action for [[tags]] line
.tm ###CHUNKDEFN### NWrof7-actO-1
.tm ###CHUNKUSE### NWrof7-awkB-2
.tm ###CHUNKUSE### NWrof7-nor6-1
.tm ###CHUNKEND###
.tm ###CHUNKBEGIN### awk program
.tm ###CHUNKUSE### NWrof7-invI-1
.tm ###CHUNKDEFN### NWrof7-awkB-1
.tm ###CHUNKDEFN### NWrof7-awkB-2
.tm ###CHUNKEND###
.tm ###CHUNKBEGIN### checkcode
.tm ###CHUNKUSE### NWrof7-reaT-9
.tm ###CHUNKUSE### NWrof7-reaT-A
.tm ###CHUNKUSE### NWrof7-reaT-B
.tm ###CHUNKDEFN### NWrof7-che9-1
.tm ###CHUNKEND###
.tm ###CHUNKBEGIN### Example run
.tm ###CHUNKDEFN### NWrof7-ExaB-1
.tm ###CHUNKEND###
.tm ###CHUNKBEGIN### functions
.tm ###CHUNKUSE### NWrof7-awkB-1
.tm ###CHUNKDEFN### NWrof7-fun9-1
.tm ###CHUNKDEFN### NWrof7-fun9-2
.tm ###CHUNKUSE### NWrof7-nor6-1
.tm ###CHUNKEND###
.tm ###CHUNKBEGIN### invoke awk program
.tm ###CHUNKUSE### NWrof7-tor6-1
.tm ###CHUNKDEFN### NWrof7-invI-1
.tm ###CHUNKEND###
.tm ###CHUNKBEGIN### noroff
.tm ###CHUNKDEFN### NWrof7-nor6-1
.tm ###CHUNKEND###
.tm ###CHUNKBEGIN### plant newline trap
.tm ###CHUNKUSE### NWrof7-tma6-7
.tm ###CHUNKDEFN### NWrof7-plaI-1
.tm ###CHUNKUSE### NWrof7-tma6-9
.tm ###CHUNKEND###
.tm ###CHUNKBEGIN### process arguments
.tm ###CHUNKUSE### NWrof7-tor6-1
.tm ###CHUNKDEFN### NWrof7-proH-1
.tm ###CHUNKEND###
.tm ###CHUNKBEGIN### read and process noweb source
.tm ###CHUNKUSE### NWrof7-awkB-1
.tm ###CHUNKDEFN### NWrof7-reaT-1
.tm ###CHUNKDEFN### NWrof7-reaT-2
.tm ###CHUNKDEFN### NWrof7-reaT-3
.tm ###CHUNKDEFN### NWrof7-reaT-4
.tm ###CHUNKDEFN### NWrof7-reaT-5
.tm ###CHUNKDEFN### NWrof7-reaT-6
.tm ###CHUNKDEFN### NWrof7-reaT-7
.tm ###CHUNKDEFN### NWrof7-reaT-8
.tm ###CHUNKDEFN### NWrof7-reaT-9
.tm ###CHUNKDEFN### NWrof7-reaT-A
.tm ###CHUNKDEFN### NWrof7-reaT-B
.tm ###CHUNKDEFN### NWrof7-reaT-C
.tm ###CHUNKDEFN### NWrof7-reaT-D
.tm ###CHUNKEND###
.tm ###CHUNKBEGIN### read back code chunk from diversion
.tm ###CHUNKUSE### NWrof7-tma6-5
.tm ###CHUNKDEFN### NWrof7-reaZ-1
.tm ###CHUNKEND###
.tm ###CHUNKBEGIN### restore environment
.tm ###CHUNKUSE### NWrof7-tma6-5
.tm ###CHUNKDEFN### NWrof7-resJ-1
.tm ###CHUNKEND###
.tm ###CHUNKBEGIN### save environment
.tm ###CHUNKUSE### NWrof7-tma6-5
.tm ###CHUNKDEFN### NWrof7-savG-1
.tm ###CHUNKEND###
.tm ###CHUNKBEGIN### set local environment
.tm ###CHUNKUSE### NWrof7-tma6-5
.tm ###CHUNKDEFN### NWrof7-setL-1
.tm ###CHUNKEND###
.tm ###CHUNKBEGIN### start document
.tm ###CHUNKUSE### NWrof7-reaT-1
.tm ###CHUNKDEFN### NWrof7-staE-1
.tm ###CHUNKEND###
.tm ###CHUNKBEGIN### tmac.w
.tm ###CHUNKDEFN### NWrof7-tma6-1
.tm ###CHUNKDEFN### NWrof7-tma6-2
.tm ###CHUNKDEFN### NWrof7-tma6-3
.tm ###CHUNKDEFN### NWrof7-tma6-4
.tm ###CHUNKDEFN### NWrof7-tma6-5
.tm ###CHUNKDEFN### NWrof7-tma6-6
.tm ###CHUNKDEFN### NWrof7-tma6-7
.tm ###CHUNKDEFN### NWrof7-tma6-8
.tm ###CHUNKDEFN### NWrof7-tma6-9
.tm ###CHUNKDEFN### NWrof7-tma6-A
.tm ###CHUNKDEFN### NWrof7-tma6-B
.tm ###CHUNKDEFN### NWrof7-tma6-C
.tm ###CHUNKDEFN### NWrof7-tma6-D
.tm ###CHUNKDEFN### NWrof7-tma6-E
.tm ###CHUNKDEFN### NWrof7-tma6-F
.tm ###CHUNKDEFN### NWrof7-tma6-G
.tm ###CHUNKDEFN### NWrof7-tma6-H
.tm ###CHUNKDEFN### NWrof7-tma6-I
.tm ###CHUNKEND###
.tm ###CHUNKBEGIN### toroff
.tm ###CHUNKDEFN### NWrof7-tor6-1
.tm ###CHUNKEND###
.tm ###ENDCHUNKS###
.tm ###BEGININDEX###
.tm ###ENTRYBEGIN### awkfile
.tm ###ENTRYDEFN### NWrof7-invI-1
.tm ###ENTRYEND###
.tm ###ENTRYBEGIN### chunkdefn
.tm ###ENTRYDEFN### NWrof7-actO-1
.tm ###ENTRYEND###
.tm ###ENTRYBEGIN### chunkuse
.tm ###ENTRYDEFN### NWrof7-actO-1
.tm ###ENTRYEND###
.tm ###ENTRYBEGIN### code
.tm ###ENTRYDEFN### NWrof7-reaT-3
.tm ###ENTRYEND###
.tm ###ENTRYBEGIN### convquote
.tm ###ENTRYDEFN### NWrof7-fun9-1
.tm ###ENTRYEND###
.tm ###ENTRYBEGIN### defn
.tm ###ENTRYDEFN### NWrof7-reaT-8
.tm ###ENTRYEND###
.tm ###ENTRYBEGIN### delay
.tm ###ENTRYDEFN### NWrof7-proH-1
.tm ###ENTRYEND###
.tm ###ENTRYBEGIN### entryarray
.tm ###ENTRYDEFN### NWrof7-actO-1
.tm ###ENTRYEND###
.tm ###ENTRYBEGIN### entrydefn
.tm ###ENTRYDEFN### NWrof7-actO-1
.tm ###ENTRYEND###
.tm ###ENTRYBEGIN### entryuse
.tm ###ENTRYDEFN### NWrof7-actO-1
.tm ###ENTRYEND###
.tm ###ENTRYBEGIN### lastdefnlabel
.tm ###ENTRYDEFN### NWrof7-reaT-8
.tm ###ENTRYEND###
.tm ###ENTRYBEGIN### lastuse
.tm ###ENTRYDEFN### NWrof7-reaT-B
.tm ###ENTRYEND###
.tm ###ENTRYBEGIN### lastxreflabel
.tm ###ENTRYDEFN### NWrof7-reaT-2
.tm ###ENTRYEND###
.tm ###ENTRYBEGIN### lastxrefref
.tm ###ENTRYDEFN### NWrof7-reaT-2
.tm ###ENTRYEND###
.tm ###ENTRYBEGIN### macrodir
.tm ###ENTRYDEFN### NWrof7-nor6-1
.tm ###ENTRYEND###
.tm ###ENTRYBEGIN### name
.tm ###ENTRYDEFN### NWrof7-reaT-8
.tm ###ENTRYEND###
.tm ###ENTRYBEGIN### noindex
.tm ###ENTRYDEFN### NWrof7-proH-1
.tm ###ENTRYEND###
.tm ###ENTRYBEGIN### tag
.tm ###ENTRYDEFN### NWrof7-fun9-2
.tm ###ENTRYEND###
.tm ###ENTRYBEGIN### tags
.tm ###ENTRYDEFN### NWrof7-actO-1
.tm ###ENTRYEND###
.tm ###ENTRYBEGIN### tagsfile
.tm ###ENTRYDEFN### NWrof7-invI-1
.tm ###ENTRYEND###
.tm ###ENTRYBEGIN### text
.tm ###ENTRYDEFN### NWrof7-reaT-4
.tm ###ENTRYEND###
.tm ###ENDINDEX###
.ENDOFDOCUMENT
