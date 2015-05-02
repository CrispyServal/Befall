#!/usr/bin/perl

open TECHIN1, "< unitdisplay.xml" or die "Cannot Open Arch File!\n";

open TECHOUT, "> unitdisplay.json";

print TECHOUT "{\n";
print TECHOUT "\"unitdisplay\" :\n";
print TECHOUT "[\n";
while(<TECHIN1>)
{
	if($_ =~ /<key>(.*)<\/key>/)
	{
		print TECHOUT "{\n";
		print TECHOUT "\"unit\" : \"$1\",\n";

		my $name = $1;

		if(<TECHIN1> =~ /<string>(.*)<\/string>/)
		{
			print TECHOUT "\"title\" : \"$1\",\n";
		}
		if(<TECHIN1> =~ /<string>(.*)<\/string>/)
		{
			print TECHOUT "\"explain\" : \"$1\", \n";
		}

		open TECHIN2, "< unitdisplayparameters.xml" or die "Cannot Open Arch File!\n";
		while (<TECHIN2>)
		{
			if($_ =~ /<key>$name<\/key>/)
			{
				print TECHOUT "\"attributes\" :\n";
				print TECHOUT "[\n{\n";
				my $buff = <TECHIN2>;
				if(<TECHIN2> =~ /<value (.*)>(.*)<\/value>/)
				{
					print TECHOUT "\"$1\" : \"$2\",\n";
				}
				if(<TECHIN2> =~ /<value (.*)>(.*)<\/value>/)
				{
					print TECHOUT "\"$1\" : \"$2\",\n";
				}
				if(<TECHIN2> =~ /<value (.*)>(.*)<\/value>/)
				{
					print TECHOUT "\"$1\" : \"$2\",\n";
				}
				if(<TECHIN2> =~ /<value (.*)>(.*)<\/value>/)
				{
					print TECHOUT "\"$1\" : \"$2\",\n";
				}
				if(<TECHIN2> =~ /<value (.*)>(.*)<\/value>/)
				{
					print TECHOUT "\"$1\" : \"$2\",\n";
				}
				if(<TECHIN2> =~ /<value (.*)>(.*)<\/value>/)
				{
					print TECHOUT "\"$1\" : \"$2\"\n}\n],\n";
				}
				$buff = <TECHIN2>;
				$buff = <TECHIN2>;

				print TECHOUT "\"consumption\" :\n[\n{\n";
				if(<TECHIN2> =~ /<value (.*)>(.*)<\/value>/)
				{
					print TECHOUT "\"$1\" : \"$2\",\n";
				}
				if(<TECHIN2> =~ /<value (.*)>(.*)<\/value>/)
				{
					print TECHOUT "\"$1\" : \"$2\",\n";
				}
				if(<TECHIN2> =~ /<value (.*)>(.*)<\/value>/)
				{
					print TECHOUT "\"$1\" : \"$2\"\n}\n]\n";
				}
			}
		}
		close TECHIN2;
		print TECHOUT "}";
		if($name =~ /longrangeunit3/)
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