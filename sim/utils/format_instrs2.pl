#!/usr/bin/env perl
#
# NOTE: This script is specially written to format specific type of files.
use strict;
use warnings;
use IO::File;
use File::Basename;
use File::Copy;
use Getopt::Long;

sub help {
    print basename($0), "\nhelp:\n";
    print "--ifile = file_name\n";
    print "--ofile = out file_name\n";
    exit 1;
}

sub print_f {
    # Local copy of file-handle
    my $l_fh;
    # Local copy of number of spaces to append
    my $l_n_s = 0;
    if(fileno $_[0]){ $l_fh = shift; }
    else            { $l_fh = *STDOUT; }

    $l_n_s = shift if($_[0] =~ /\d+/);
    my $sp = "";
    $sp .= " " for (0..$l_n_s);
    return if($#_ < 0);
    foreach (@_){ print $l_fh $sp, $_ ; }
}

my $opt_fname;
my $opt_help;
my $opt_ofname;
GetOptions('ifile=s' => \$opt_fname, 'ofile=s' => \$opt_ofname, 'help|?' => \$opt_help) or help;
help if defined $opt_help;
print "No file name passed\n" and exit 1 if not defined $opt_fname;

my $fh = new IO::File;
my $fho = new IO::File;
my $o_fname = basename($opt_fname);
my $o_f_basename;
my $tmp_file;

# Remove file extension from the file
$o_fname =~ s/\.[\w]*//;
# Get basename of the file ( i.e without the extension )
$o_f_basename = $o_fname;
$o_fname .= ".h";
$tmp_file = "/tmp/" . $o_f_basename . ".tmp";

# If output file name was specified on command line, use that instead
$o_fname = $opt_ofname if(defined $opt_ofname);
# Create a file backup, if it already exists.
#copy("$o_fname", "${o_fname}.org") or die "Copy failed: $!" if( -e $o_fname);

my $n_instrs = 0;
# If this flag is set, means we have to ignore this opocde
my $ignore_this = 0;
# This line
my $line_this;

#----------------------------------------------------------------------------------------
# function'a name
my $ppc_func_name = "gen_ppc_opc_func_hash";

# C macros defined here ( imp )
#
# Args
my $ppc_ic_arg0 = "\"ic->arg[0].p\"";   # -DARG0
my $ppc_ic_arg1 = "\"ic->arg[1].p\"";   # -DARG1
my $ppc_ic_arg2 = "\"ic->arg[2].p\"";   # -DARG2
my $ppc_ic_arg3 = "\"ic->arg[3].p\"";   # -DARG3
my $ppc_ic_arg4 = "\"ic->arg[4].p\"";   # -DARG4
my $ppc_ic_arg5 = "\"ic->arg[5].p\"";   # -DARG5

my $cpu_ppc_umode = "\"uint32_t\"";   # -DUMODE
my $cpu_ppc_smode = "\"int32_t\"";    # -DSMODE

my $cpu_ppc_name  = "\"cpu\"";

# create some of the macros for gcc command line
my $ppc_macros_list = " -DARG0="   . $ppc_ic_arg0   . " -DARG1="   . $ppc_ic_arg1   .
                      " -DARG2="   . $ppc_ic_arg2   . " -DARG3="   . $ppc_ic_arg3   .
                      " -DARG4="   . $ppc_ic_arg4   . " -DARG5="   . $ppc_ic_arg5   .
                      " -DUMODE="  . $cpu_ppc_umode . " -DSMODE="  . $cpu_ppc_smode .
                      " -Dcpu="    . $cpu_ppc_name  ;

# defines going directly to the file
my $cpu_ppc_defines = [ "#define reg(x) (*((uint64_t *)(x)))" ];


#-----------------------------------------------------------------------------------------


# List of opcodes to be ignored.
# Hairdcoded
my @opc_ignore = ("to_be_translated");

# Preprocess the source file with gcc first and create a temporary file.
system("gcc -E $ppc_macros_list $opt_fname &> $tmp_file");

# Open the temporary file ( created above ) and use it as source.
$fh->open("<$tmp_file");
$fho->open(">$o_fname");

# Insert all defines into the file
foreach (@$cpu_ppc_defines) {
    print_f $fho, 0, $_, "\n";
}
# Start the function definition
print_f $fho, 0, "void $ppc_func_name(cpu_ppc_booke *cpu){\n\n";
#print_f $fho, 4, "typedef void (*ppc_opc_fun_ptr)(cpu_ppc_booke *, struct instr_call *);\n";
#print_f $fho, 4, "static std::map<std::string, ppc_opc_fun_ptr>  ppc_func_hash;\n"; 

WHILE_00: while(<$fh>){
    my @l_stack_fun = ();
    my @lines = ();
    $line_this = $_;

    if($line_this =~ /X\(([\w\.\s]+)\)/){
        
        my $count;
        my $found_braces = 0;
        my $found_left_brace = 0;
        my $found_right_brace = 0;
        
        my $opc = $1;
        # Reset ignore flag
        $ignore_this = 0;
        # Reset the stack
        @l_stack_fun = ();

        # Check for $opc in @opc_ignore and skipover to next opcode, if match is found.
        $ignore_this = 1 if (grep { $opc eq $_ } @opc_ignore);
        
        # Increase instruction count
        $n_instrs++ if ($ignore_this == 0);

        # Remove opcode ( with brackets ) from this line.
        $line_this =~ s/X\([\w\.\s]+\)//g;

        # See how many braces are there on a single line.
        if($count = () = ($line_this =~ /(\{)/g)) { for (1..$count) { $found_braces++; push @l_stack_fun, $1;} $found_left_brace++; }
        if($count = () = ($line_this =~ /(\})/g)) { for (1..$count) { $found_braces++; pop @l_stack_fun; } $found_right_brace++; }
        # If all the braces are on single line then jump over
        push @lines, $line_this;

        if($#l_stack_fun == -1 and ($found_braces > 0) and (($found_braces % 2) == 0) and ($found_left_brace == $found_right_brace) ){ goto AFTER_WHILE_0; }
        WHILE_0: while(<$fh>){
            $line_this = $_;
            $line_this =~ s/\t/    /g;
            # Allow perl like comments
            $line_this =~ s/#/\/\// if ($line_this =~ /^\s*#/);

            # Keep track of braces
            if($count = () = (/(\{)/g)) { for (1..$count) {push @l_stack_fun, $1;} }
            if($count = () = (/(\})/g)) { for (1..$count) {pop @l_stack_fun;} }
            push @lines, $line_this;
            if($#l_stack_fun == -1){ goto AFTER_WHILE_0; }
        } 
        AFTER_WHILE_0:
        # If ignore flag set set, tha means we have to ignore this opocde.
        if($ignore_this == 0){
            my $opc_func = $opc;
            $opc_func =~ s/\./_dot/g;   # substitute all .'s with dot
            print_f $fho, 4, "// $opc\n";
            print_f $fho, 4, "struct ___${opc_func}___ {\n";
            print_f $fho, 8, "static void ${opc_func}___(cpu_ppc_booke *cpu, struct instr_call *ic)";
            print_f $fho, 8, @lines;
            print_f $fho, 4, "};\n";
            print_f $fho, 4, "cpu->ppc_func_hash[\"$opc\"] = ___${opc_func}___\:\:${opc_func}___;\n";
            print_f $fho, 4, "\n";
        }
    }elsif (not /^\s*$/){       # skip over blank lines
        # Allow perl like comments
        $_ =~ s/#/\/\// if (/^\s*#/);
        print_f $fho, 4, $_;
    }
}

print_f $fho, 0, "}\n";

# close all files
$fh->close;
$fho->close;
