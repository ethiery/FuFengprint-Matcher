#!/bin/bash

# rename 's/Dan1439814943-Hamster/004-/' Dan1439814943-Hamster*
# rename 's/Dan1439815405-Hamster/005-/' Dan1439815405-Hamster*
# rename 's/Dan1439818077-Hamster/006-/' Dan1439818077-Hamster*
# rename 's/Dan1439818838-Hamster/007-/' Dan1439818838-Hamster*
# rename 's/Dan1439819825-Hamster/008-/' Dan1439819825-Hamster*
# rename 's/Dan1439885084-Hamster/009-/' Dan1439885084-Hamster*
# rename 's/Dan1439887504-Hamster/010-/' Dan1439887504-Hamster*
# rename 's/Jam1458281834-Hamster/011-/' Jam1458281834-Hamster*
# rename 's/Jam1458283207-Hamster/012-/' Jam1458283207-Hamster*
# rename 's/Jam1458283896-Hamster/013-/' Jam1458283896-Hamster*
# rename 's/Jam1458286356-Hamster/014-/' Jam1458286356-Hamster*
# rename 's/Jam1458286598-Hamster/015-/' Jam1458286598-Hamster*
# rename 's/Jam1458286990-Hamster/016-/' Jam1458286990-Hamster*
# rename 's/Jam1458287213-Hamster/017-/' Jam1458287213-Hamster*
# rename 's/Jam1458287744-Hamster/018-/' Jam1458287744-Hamster*
# rename 's/Jam1458288272-Hamster/019-/' Jam1458288272-Hamster*
# rename 's/Jam1458288735-Hamster/020-/' Jam1458288735-Hamster*
# rename 's/Jam1458289244-Hamster/021-/' Jam1458289244-Hamster*
# rename 's/Jam1458289646-Hamster/022-/' Jam1458289646-Hamster*
# rename 's/Jam1458290130-Hamster/023-/' Jam1458290130-Hamster*
# rename 's/Jam1458290309-Hamster/024-/' Jam1458290309-Hamster*
# rename 's/Jam1458290617-Hamster/025-/' Jam1458290617-Hamster*
# rename 's/Jam1458291050-Hamster/026-/' Jam1458291050-Hamster*
# rename 's/Jam1458292348-Hamster/027-/' Jam1458292348-Hamster*
# rename 's/Jam1458293135-Hamster/028-/' Jam1458293135-Hamster*
# rename 's/Jam1458293487-Hamster/029-/' Jam1458293487-Hamster*
# rename 's/Jam1458294016-Hamster/030-/' Jam1458294016-Hamster*
# rename 's/Jam1458294172-Hamster/031-/' Jam1458294172-Hamster*
# rename 's/Jam1458294413-Hamster/032-/' Jam1458294413-Hamster*
# rename 's/Jam1458294871-Hamster/033-/' Jam1458294871-Hamster*
# rename 's/Jam1458295305-Hamster/034-/' Jam1458295305-Hamster*
# rename 's/Jam1458295702-Hamster/035-/' Jam1458295702-Hamster*
# rename 's/Jam1458296154-Hamster/036-/' Jam1458296154-Hamster*
# rename 's/Raz1439798405-Hamster/037-/' Raz1439798405-Hamster*
# rename 's/Raz1439810088-Hamster/038-/' Raz1439810088-Hamster*
# rename 's/Raz1439812499-Hamster/039-/' Raz1439812499-Hamster*
# rename 's/Raz1439821435-Hamster/040-/' Raz1439821435-Hamster*

for suffix in 001 002 003 004 005 006 007 008 009 010 011 012 013 014 015 016 017 018 019 020 021 022 023 024 025 026 027 028 029 030 031 032 033 034 035 036 037 038 039 040
do
	for fingerNo in 0 1 2 3 4 5
	do
		letter=a
		for f in ${suffix}-${fingerNo}*
		do
			mv ${f} ${suffix}-${fingerNo}${letter}.fmr
			letter=$(echo "$letter" | tr "0-9a-z" "1-9a-z_")
		done
	done
done