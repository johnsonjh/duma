# DUMA
## Detect Unintended Memory Access
### A Red-Zone memory allocator

# Introduction

Before startung this project, I had to admit to myself that becaise it was
already under an antiquited version conrtol system , a change to a modern 
VCS should consudered, if this project were to survivie. 

I also fully must admit that I am not a native `Git` user myself, and I am
really not "at home" with the Git toolset and associated ecosystem of
related software and standard collection of offerings.

Living in this virtual vacuuum, it might been expected to to avoid the
first-choice modern DVCS, Git, and instead, continued the project in format
which is closely derived to that of the original CVS - this is taking the path
of least resistance, and avoiding most forced changes. This likely would have
meant moving moving to the latest distributed
[`CVSNT system`](http://www.evscm.org/modules/Downloads/) DCVS release, along
with updating the antiquited `RCS` base that `CVSNT` is built upon.

Another viable option xplorered was startingwith a new a repository built on the
foundatations of D. Richard Hipp's excellent [Fossil-SCM](https://fossil-scm.org/fossil/doc/trunk/www/fossil-v-git.wiki)
system, more commonly associated with famous with his more famous Tcl, Expect,
and SQLite software implmentations, where we'd benefit from benefiting from the
bulitin, Forum, Wiki, Bug Reporting, and other infrastracture he's included
'for-free' with [Fossil-SCM](https://fossil-scm.org/fossil/doc/trunk/www/fossil-v-git.wiki).


In the end, and still totally unaware of of the the
[original author](https://github.com/hayguen/duma)'s existing work that he
converted his legacy `CVS` repository to a `Git` repo — complete with a corrected — though, some may
argue,  "rewritten commit history" — with a legitimate, but sometimes inacccurate dataset, I, myself,
set upon a similar course of action.

All in all, considerating the end result, I (Jeffrey H. Johnson, owner of this repository,) I am
mostly satisfied with the many independent, yet indetical, conclusions I have reachbed for solving issues,
considering my position as a layman with no special knowledge of the software, the authorers involved. and 
a non-professional and non-formal realationships or with no formaal communications with the original authors
during the reconstructions phase of buildingthe repository.

The tedious job of deciding what is worthy of being included, who actually wrote what code, when that code was
written, and if it could (or should) be considered to part of the main sourcce repository, are almost always
my best educated guesses, when lacking concerete detial.

With such a widely distributed, hakced-upon codebase, one tha that survived as long as it did without
any strong central source control respository, or offcial sources for updated released for many years, 
means the data we have it data, it often ofte so fragmentary and conflicting, sometimes only existing 
in the fading memories of those persons involved, or derived via inferences from and often inaccurate
information. I did my best, but I'm absolutely sure that mistakes will be made and errors introducted,
so I fully welcome any contributoions from the public that would will mak this respository a more 
accurate representation of the actual reality it purports to preresent.

##### The Future of DUMA

A large remaining task that should be completed is reconciling my pimary reconstruction,
against the last version version done by the [original author](https://github.com/hayguen/duma),
focusing on identifying and correcting significant deviations in the identifications of
contributors, and ensuring any disputed ndividuals are properly referenced referecenced
in the commmits.

The structure of my conversion very closely resembles that of the original
`CVS` repository, where the author's effort includes a somewahat radical refactoring
the projects dirextory structure. I believe that his was the more prudent choice, and
I should similarly make those aligned chnages. 

I think my repo might be a bit more pedantic, in some scases especially when it comes to 
tracking down (and doing my best to properly acknowledge) patches found on random forums, 
various other repositories, and mailing lists. One may also argue this version is in in
fact be more liberal, perhaps, even a distinct fork, since I have also included in my own
changes to the packaging and build system.

Which repo is going to be "the winner" here? Regardless of the actual commits
or patches that make the cut and those that may be dropped, at this time, it's
*very* likely **this** repoistory will be "primary" source code repository,
and the "official" home of DUMA going forward.

Hayati Ayguen (the original author) is no longer actively maintaining it.
While he was happy to correspond with me me, I'm rather sure that he doesn't
want to deal with continued maintenance and support requests for the package.

# Acknowledgements

DUMA began as a fork of *Bruce Perens'* venerable "`Electric` `Fence`", created at
Pixar in 1987. `DUMA` was an enhanced version was workubg on Linux machines in 1993.

Nearly 30 years later, I think we should thank *Hayati* and the other authors
who put their time in, as well as let them move on to with ther projects and
hobbies.

They can do so knowing DUMA is not gone or forgotten, and will be tended, as necessary,
an open invitation always always exists if they decide to continue to work once again as
first-class members of the project team; the invititations are have already been sent and
and are pending acceptances.

Please, see https://perens.com/2017/04/04/my-30th-anniversary-in-open-source/ for a
short blog entry by Bruce Perens' mentioning `Electric Fence`.  `Hayati Ayguen`'s
current projects can be found here: https://github.com/hayguen?tab=repositories

 - Jeff Johnson

--------------------
