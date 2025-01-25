use strict;
use warnings;
use JSON::XS;
use File::Slurper 'read_binary';

my %repos;

for my $repo (glob "repositories/*") {
    my %issues;
    for my $issue (glob "$repo/issues/*") {
        $issues{$issue} = decode_json(read_binary($issue));
    }
    for my $pull (glob "$repo/pulls/*") {
        $issues{$pull} = decode_json(read_binary($pull));
    }
    $repos{$repo} = \%issues;
}

for my $repo (sort keys %repos) {
    my $count = keys %{$repos{$repo}};
    print "$repo: $count\n";
}
