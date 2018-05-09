#!/usr/bin/env perl
#
# declare and define/init test-tool hashes (of hashes):
# as our runtime manifest -- perl statements to be eval'd...
#
######################################### start of astro-h unit test manifest ##################################
#
# please do NOT change this hash variable name!
#
our %runtime_manifest = (
#
# the manifest provides direction to aht.pl regarding specifics
# of input and output files, including logs. the testheader should indicate something
# about the nature of the test and who when where it ius/was conducted.
# typically expect the testheader to be set at aht runtime via "aht.pl -i or -u"
# (or "aht.pl --init or --update"). but the developer can override updates by manually
# editing and setting the final attibute to "true" -- which indicates update 
# should not reset this manifest entry:
testheader => {
  final => "true",
  describe => "testheader: indicates version, who, where, when", 
  default => "version 1.0, dhon, gs66-xen.gsfc.nasa.gov, July 2012",
  valvec => ["version 1.0", "dhon", "gs66-xen.gsfc.nasa.gov", "July 2012"]
},
#
# support either (t)csh or (ba)sh login/runtime...
# the indicated file should establish all required tool runtime 
# environment variables (PATH, LD_LIBRARY_PATH, PERL* etc.)
setup => {
  final => "true",
  describe => "setup: runtime env. in (t)csh or bash",
  default => "./etc/setup.sh",
  valvec => ["./etc/setup.csh", "./etc/setup.sh"]
},
#
# aht.pl must be informed of the specific tool apps this manifest supports
# invoking "aht.pl -t" or "aht.pl -toolname" with the name of an existing
# binary app. will run the app. as well as updating this manifest entry with
# a new "default" app. path-name. i.e. "apt.pl -t foo" should indicate the
# foo binary currently residing in ./bin/.
toolname => {
  final => "true",
  describe => "toolname: binary executable to run/test",
  default => "hxisgdsff",
  valvec => ["hxisgdsff"]
},
#
# aht.pl should pass along any cmd-line options it does not recognize as its own
# to the tool binary app. it executes/tests. if the tool app. is happy with
# the provided cmd-line options, aht.pl may update this manifest entry (maybe?) 
toolcmdopts => {
  final => "true",
  describe => "toolcmdopts: optional command-line options to feed binary executable",
  default => "mode=h clobber=yes",
  valvec => ["mode=h", "clobber=yes", "mode=h clobber=yes"]
},
#
# a non-zero timeout informs aht.pl that the tool app. should be expected to
# run to completion within some given time frame. aht.pl may update this manifest entry.
tooltimeout => {
  final => "true",
  describe => "tooltimeout: in seconds -- 0 indicates forever",
  default => "20",
  valvec => ["20", "10", "5", "2"]
},
#
# tool exit status codes
exitstatus => {
  final => "true",
  describe => "exitstatus: expected status codes for success and known error / failure modes.",
  default => "0",
  valvec => ["0", "1", "2", "3", "4", "5"]
},
#
# the full directory-path to the calibration database shall be explicitly indicated here
# or perhaps via an environment variable or some such combination?
caldb => {
  final => "false",
  describe => "caldb: path to the calibration database used by the fits apps. to be tested",
#  default => "${CALDB}",
  default => "./caldb",
  valvec => ["./caldb"]
#  valvec => ["./caldb", "${CALDB}", "${HEADAS}/caldb"]
},
#
# the update ("aht.pl -u") behavior triggers a directory scan of the input directory(ies)
# with the results placed into this entry -- the default will typicall be the first file found.
input => {
  final => "true",   
  describe => "input: location and names of data and ancillary files",   
  default => "03ahSGD_tlm.fits", 
  valvec => ["03ahSGD_tlm.fits", "SGD_FF2012June.tpl", "SGD_SF2012June.tpl"]
},   
#   
# the update ("aht.pl -u") behavior triggers a directory scan of the output directory(ies)   
# that results in a copy of all currently available files to the expected_output   
# directory(ies), and an update/reset of this entry.   
# note that the filenames will be prefixed automatically with "./expected_output/hostname.domainname/"   
expected_output => {   
  final => "true",   
  describe => "expected_output: files that the test shall compare to the runtime results/outputs for validation",   
  default => "03ahSGD_tlm_extract.fits",   
  valvec => ["03ahSGD_tlm_extract.fits"]   
},   
#   
# post procesing validation should include searches for specific keywords in output log   
# files (stdout and stderr). this manifest entry must be manually edited and is not affected   
# by an update. consequently it is always final.   
logkeywords => {   
  final => "true",   
  describe => "logkeywords: optional list of keyword to seach in logfiles.",   
  default => "all",   
  valvec => ["success", "* warning", "** error", "*** fatal"] 
}   
); # runtime_manifest hash 
#   
######################################### end of astro-h unit test manifest ####################################   
#   
# the aht.pl test driver supports its own cmd-line options   
# as expressed in the following perl statment to be eval'd   
# our aht cmd-line options:   
our %aht_cmdopts = (   
help => {   
  describe => "help: -h or or [-]-help synopsis and related info.",   
  default => "brief",   
  valvec => ["brief", "verbose", "veryverbose"]   
},   
#    
tooltest => {   
  describe => "tooltest: -t or [-]-tool or [-]-toolname [ftool name] binary executable of test -- ftool cmdline options",   
  default => "false",   
  valvec => ["test_tool_name", "run the test on the indicated tool"]   
},    
#   
filesys => {   
  describe => "filesys: -f filesystem info. used by init and for runtime test",   
  default => "all",   
#  valvec => [ "./log", "./etc", "./lib", "./bin", "./input", "./output", "./output/stderr", "./output/stdout", "./expected_output", "./expected_output/stderr", "./expected_output/stdout"]   
  valvec => [ "./log", "./etc", "./input", "./output", "./output/stderr", "./output/stdout", "./expected_output", "./expected_output/stderr", "./expected_output/stdout"]   
},   
#   
init => {   
  describe => "init: -i or [-]-init [ftool name] initialize filesystem and create working manifest, and implicit update if possible.",   
  default => "testheader",   
  valvec => ["teastheader", "init the test manifest of this tool"]   
},    
#   
debug => {   
  describe => "debug: -d or [-]-debug [ftool name] exec gdb debugger on current tool binary.",   
  default => "false",   
  valvec => ["false", "true"]   
},    
#   
env => {   
  describe => "env: -e or [-]-env print runtime environment of test.",   
  default => "false",   
  valvec => ["false", "true"]   
},    
#   
noop => {   
  describe => "noop: -n or [-]-noop [ftool name] print runtime execution info.",   
  default => "false",   
  valvec => ["false", "true"]   
}, 
#   
param => { 
  describe => "param: -p or [-]-par [ftool name] print runtime tool parameter info.",   
  default => "false",   
  valvec => ["false", "true"]   
},    
#   
update => {   
  describe => "update: -u or [-]-update [ftool name] recursively copy latest -- good -- test results from runtime ./output subdir to ./expected_output.",   
# the update option has become complicated a bit by the requirement to segregate tests by hostname and date-time.   
# assume the most recent output is something like:  ./hostname/YearMonDay/Hour.Min.Sec/output"   
# and ./output is a symlink to it, and update should recursively copy all ./output to ./expected_output.    
  default => "false",   
  valvec => ["false", "true"] 
}, 
# 
verbose => { 
  describe => "verbose: -v[v] or [-]-[very]verbose print verbose dscription of runtime test activities.",   
  default => "false",   
  valvec => ["false", "true"] 
},    
#   
manifest => {   
  describe => "manifest: -m indicate manifest-file of test",   
  default => "false",   
  valvec => ["false", "true"] 
}   
); # aht_cmdopts hash 
