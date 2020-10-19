# DUMA
## Detect Unintended Memory Access
### A Red-Zone memory allocator

# Introduction

Before starting this project, was clear that the antiquated CVS version
control system would be a limiting factory, any revived DUMA distribution
would need to use a modern and common DVCS, (if the project was going to
survive in the long term).

I also will fully admit that I am not an expert `Git` user, by any means,
and do not feel"at home" with the Git package itself and the associated
tools in the ecosystem of Git-related software which makes up the current
standard offerings.

Living in my VCS vacuum, it might have been expected that I would avoid the
first-choice modern DVCS, Git, and instead, continue the project with a VCS
closely derived from that of the original CVS - this would be taking the path
of least resistance, and avoiding a the pain of learning a new systems and
invasive changes. 

This likely would have meant moving moving to the last distributed [`CVSNT system`](http://www.evscm.org/modules/Downloads/) release,
along with updating the antiquated `RCS` base that `CVSNT` is built upon.

Another more viable option that was explored was migration to a mew repository
built on a foundation based on of D. Richard Hipp's excellent [Fossil-SCM](https://fossil-scm.org/fossil/doc/trunk/www/fossil-v-git.wiki)
system. While more commonly associated with his famous Tcl, Expect, and SQLite,
software, he also is the author and maintainer of this excellent VCS system,
currently used to maintainl SQLite. We would benefit the many built-in Fossil
features such as the Forum, Wikis, Bug Reporting, and other infrastructure
DRH has included 'for-free' with [Fossil-SCM](https://fossil-scm.org/fossil/doc/trunk/www/fossil-v-git.wiki).

However, in the end, completely and totally unaware of of the
[original author](https://github.com/hayguen/duma)'s existing work converting
his legacy `CVS` repository to a `Git` repository — complete with a corrected —
though, some may argue "rewritten commit history", providing a legitimate but
sometimes inaccurate dataset.

I set upon a similar course of action, converted the `CVS` to repository to
`Git` completely independently, along with identifying and merging so many
community contributions that had been added over a period of many years.

All in all, considering the end result, I (Jeffrey H. Johnson, owner of this
repository), can say that I am mostly satisfied, as many of my independent
conclusions I reached to solve various issues, were nearly identical to the
author's own conversion efforts.  It must be known that I am layman with no
special knowledge of thos software, nor any special relationship with the
author.

I am simply a non-professional user of the software, who maintained
no formal communication with the original authors during the reconstruction
phase of building the repository.

It was a tedious job of deciding what was worthy of being included, and the
figuring out who actually wrote the code, when the code was written, and if 
it could (or should) be considered as part of the main source repository. 

These are almost always educated guesses, and, when lacking concrete detail,
judgment calls. With such a widely distributed, hakced-upon code-base, one
that that survived for as long as it did, without any strong central source
control repository, or official releases for many years, means the data we
have is often fragmentary and conflicting, sometimes only existing in the
fading memories of those persons involved, or derived via inferences from 
what turns out to be inaccurate information, all too often.

While I did my best, I'm absolutely certain that mistakes were made and 
errors introduced, so I fully welcome any contributions from the public
that would help to make this repository a more accurate representation of
reality it purports to represent.

# The Future of DUMA.

A large remaining task that must be completed is reconciling my primary
reconstruction, against the last version which was released by the
[original author](https://github.com/hayguen/duma), focusing on identifying
and correcting any significant deviations in the identifications of
contributors, and ensuring any disputed individuals are properly referenced
in the commit logs.

The structure of my conversion very closely resembles that of the original
`CVS` repository, where the author's efforts included a somewhat a radical
refactoring of the directory structure. I believe that his was the more 
prudent choice, and I should eventually move to make similar changes in this
repository.

This repository may be a bit more pedantic in some cases, especially when it
comes to tracking down (and doing my best to properly acknowledge) patches
found on random forums, in various other repositories, and on mailing lists.

One may could also argue this version is actaully be more "liberal", maybe
even a distinct fork, since I have included my own changes to the packaging,
and extended the build system in ways the original author did not.

# Which repository is going to be "the winner" here? 

Regardless of which actual commits and patches make the cut and (those that
may be dropped), at this time, it's *very* likely **this** repository will be
the "primary" source code repository, and the new "official" home of D.U.M.A. 
going forward.

Hayati Ayguen (the original author) is no longer actively maintaining D.U.M.A.

While he was happy to correspond with me, I belive it was clear he doesn't 
want to want to deal with continued maintenance and support requests for
this package.

# Acknowledgements

D.U.N.A. started out as a fork of *Bruce Perens'* venerable "`Electric Fence`",
created at "Pixar" back in 1987. `D.U.M.A.` was an enhanced version, 
working on Linux machines as early as 1992. So, now, nearly 30 years later,
I think we should thank *Hayati* and the other contributors who put their
time amd efforts into this project, but let them move on to other projects
and hobbies.

They will do so knowing D.U.M,A. is not gone or forgotten, that it will be
tended to, as necessary. As is customary in the world of mature, open source
software, an open-ended invitation has been officially and permanently
extended, should they decide to continue take the helm as the primary 
maintainers or contributors again for the DUMA project; the official invitation
has already been extended and (and some acceptances still are pending), if 
that is the will and desire of the original authors.

Please, see https://perens.com/2017/04/04/my-30th-anniversary-in-open-source/
for a short blog entry by Mr. Bruce Perens' mentioning `Electric Fence`. 

Also, `Hayati Ayguen`'s current projects can be found here:

https://github.com/hayguen?tab=repositories


 -- Jeff Johnson


--------------------
