#!/usr/bin/perl -w

my $magic_from='0x7c08cba6'; # mtspr 808,r0 -- spr808 has no name, instr is nop.
my $magic_to='0x7c09cba6'; # mtspr 808,r0 -- spr808 has no name, instr is nop.

my $expect=''; # state of our little FSM

my @keep=();
my @pass2=();
my %xlate;

while (<>) {
    push @keep,$_;

    if (/amxlc_tbl__(.*)__(\d+)__endentry/) {
	$xlate{$2}=$1;
    }

    if ($expect eq '' && /^^\s*b\s+/) {
	# might be one of our marks
	$expect='mtspr';
	next;
    }

    if ($expect eq 'mtspr' && /$magic_from/) {
	# expect line number to be encoded in next line
	$expect='addi';
	$append=1;
	next;
    }

    if ($expect eq 'mtspr' && /$magic_to/) {
	# expect line number to be encoded in next line
	$expect='addi';
	$append=0;
	next;
    }

    if ($expect eq 'addi' && /addi[^,]*,[^,]*,(\d+)/) {
	my $line=$1;
	if ($append) {
	    push @keep,"#amxlc_label_lookup: $line\n";
	}
	else {
	    unshift @keep,"#amxlc_label_lookup: $line\n";
	}
        goto flush;
    }

flush:
    $expect='';
    push @pass2,@keep;
    @keep=();
}

push @pass2,@keep;

for (@pass2) {
    if (/^#amxlc_label_lookup: (\d+)/) {
	my $label=$xlate{$1};
	$_=defined($label)?"#$label\n":"#AMXLC LABEL LINE $1\n";
    }
    print;
}
