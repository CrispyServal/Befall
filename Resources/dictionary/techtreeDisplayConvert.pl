#!/usr/bin/perl

open TECHIN1, "< techtreedisplay.xml" or die "Cannot Open Arch File!\n";

open TECHOUT, "> techtreedisplay.json";

print TECHOUT "{\n";
print TECHOUT "\"techtree\" :\n";
print TECHOUT "[\n";
while(<TECHIN1>)
{
	if($_ =~ /<key>(.*)<\/key>/)
	{
		print TECHOUT "{\n";
		print TECHOUT "\"tech\" : \"$1\",\n";

		my $name = $1;

		if(<TECHIN1> =~ /<string>(.*)<\/string>/)
		{
			print TECHOUT "\"title\" : \"$1\",\n";
		}
		if(<TECHIN1> =~ /<string>(.*)<\/string>/)
		{
			print TECHOUT "\"explain\" : \"$1\", \n";
		}
		if(<TECHIN1> =~ /<string>(.*)<\/string>/)
		{
			print TECHOUT "\"dialogue\" : \"$1\", \n";
		}
		if(<TECHIN1> =~ /<string>(.*)<string>/)
		{
			print TECHOUT "\"imagename\" : \"$1\", \n";
		}

		open TECHIN2, "< techtreeparameters.xml" or die "Cannot Open Arch File!\n";
		while (<TECHIN2>)
		{
			if($_ =~ /<key>$name<\/key>/)
			{
				print TECHOUT "\"fathers\" :\n";
				print TECHOUT "[";

				while (<TECHIN2>)
				{
					if($_ =~ /<father>(.*)<\/father>/)
					{
						print TECHOUT "\n\"$1\"";
						if(<TECHIN2> =~ /<father>(.*)<\/father>/)
						{
							print TECHOUT ",\n\"$1\"";
						}
						last;
					}
				}
				print TECHOUT "\n],\n";

				print TECHOUT "\"influence\" :\n{\n";
				while (<TECHIN2>)
				{
					if($_ =~ /<value (.*)>(.*)<\/value>/)
					{
						my $value;
						my $type;
						my $target;
						$value = $2;
						if($2 == "true")
						{
							$value = 1;
						}
						$type = $1;
						if($1 =~ /(.*)LongRange(.*)/)
						{
							$target = "longRange";
							$type = "$1$2";
						}
						if($1 =~ /(.*)ShortRange(.*)/)
						{
							$target = "shortRange";
							$type = "$1$2";
						}
						print TECHOUT "\"type\" : \"$type\",\n\"target\" : \"$target\",\n\"value\" : $value\n},\n";
						last;
					}
				}
				my $buff = <TECHIN2>;
				$buff = <TECHIN2>;

				print TECHOUT "\"consumption\" :\n{\n";
				if(<TECHIN2> =~ /<value (.*)>(.*)<\/value>/)
				{
					print TECHOUT "\"$1\" : $2,\n";
				}
				if(<TECHIN2> =~ /<value (.*)>(.*)<\/value>/)
				{
					print TECHOUT "\"$1\" : $2,\n";
				}
				if(<TECHIN2> =~ /<value (.*)>(.*)<\/value>/)
				{
					print TECHOUT "\"$1\" : $2\n}\n";
				}
			}
			
		}
		close TECHIN2;
		print TECHOUT "}";
		if($name =~ /defbase2/)
		{
			print TECHOUT "\n";
		}
		else
		{
			print TECHOUT ",\n";
		}
	}
}

print TECHOUT "\n]\n}\n";
close TECHIN1;
close TECHOUT;