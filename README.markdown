xmlscrambler
============

This is a simple utility for "scrambling" the contents of an XML document
while preserving the structure of the XML. It only modifies the text nodes
or the stuff _between_ the tags. Anything inside the tags (including
attribute values) is preserved as-is.

The motivation for writing this was to allow customers with sensitive
content in their XSL-FO documents to be able to send us samples that were
useful for testing and debugging. 

This is actually the third iteration (that I know of) of an XML scrambler
developed at Antenna House. It was written to solve two problems:

1.	Scrambling large documents. The other two scramblers both used a DOM
	parser so they needed to load the entire document into memory before
	they could operate on it. By contrast, my implementation uses Expat
	which is a stream parser. It only loads a small chunk of the document
	at a time.
1.	Security. Primarily I was trying to answer the question, "how can we
	be sure it's truly random"?

The security question is one that I had given some thought and discussed
with my coworker, Alex, several times. How do we (or our customers) know
it's _really_ random? I was only able to find the source code for one of
the two scramblers we were sending out to customers. And without being
able to see the source, it would be difficult to determine if the output
being produced was really random or merely encrypted with a reversible
algorithm of some sort. Something as simple as rot-13 would appear random
to the untrained eye, but is, in fact, trivial to reverse. We need a way
for customers to verify beyond any doubt that the document was
irrecoverably scrambled. This led me to the decision that instead of
"scrambling" the data, what I should be doing is "masking" the data.

The way the current "scrambler" works is by replacing all uppercase latin
characters with 'X', all lowercase latin characters with 'x', and all
digits with '0'. All other characters are passed through unmolested. This
allows anyone to, at a glance, verify that the document has been securely
stripped of sensitive information while still producing a document that
is (usually) suitable for our testing.
