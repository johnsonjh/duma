## DUMA
### Detect Unintended Memory Access
#### A Red-Zone memory allocator

##### Introduction

Unaware of the [original author](https://github.com/hayguen/duma)'s similar work converting
his legacy `CVS` repository to `git` — complete with corrected, though, some
would argue 'modified' - commit history to ensure historical contributors
retained their acknowledgements and authorship attributions, I (Jeffrey H.
Johnson), completely independently undertook and (mostly) this project.

The tedious task of deciding what should be included, who wrote it, when
it could (or even should) be considered as part of the mainline code, and
when that all actually took place is often judgement call - or just a guess.

Such a widely distributed and hakced-on codebase, one without strong central
control, and without regular updates for many years means some extant data is
fragmentary and conflicting, sometimes only existing in the fading memories of
those involved, or determinee by inferences made with incomplete and sometimes
inaccurate information. I did my best, but, I'm absolutely sure that mistakes
were made and errors introducted, and I welcome any additions or corrections.

##### The Future of DUMA

The first big task should be reconciling my reconstruction against the version
done by the [original author](https://github.com/hayguen/duma), focusing on 
identifying and correcting significant deviations in the identifications of
contributors, and ensuring these individuals are properly referenced.

The structure of this conversion very closely resembles that of the original
`CVS` repository, where the author's effort included refactoring the projects
dirextory structure. I believe that was a prudent choice and should similarly
make those chnages. I think this repo might be a bit more pedantic, especially
when it comes to tracking down (and doing my best to properly acknowledge)
patches found on random forums, various other repositories, and mailing lists.
One may also argue this version to be more liberal, even a distinct fork,
since I have also included in my own changes to the package and build system.

Which repo is going to be "the winner" here? Regardless of the actual commits
or patches that make the cut and those that may be dropped, at this time, it's
*very* likely **this** repoistory will be "primary" source code repository,
and the "official" home of DUMA going forward.

Hayati Ayguen (the original author) is no longer actively maintaining it.
While he was happy to correspond with me me, I'm rather sure that he doesn't
want to deal with continued maintenance and support requests for the package.

# Acknowledgements

DUMA began as a fork of Bruce Perens' venerable "Electric Fence", created at
Pixar in 1987. DUMA was an enhanced version that worked on Linux by 1993.

Nearly 30 years later, I think we should thank Hayati and the other authors
who put their time in, bit let them move on to new projects and hobbies.

They can do so knowing DUMA is not gone or forgotten, and will be tended to
when necessary.

See https://perens.com/2017/04/04/my-30th-anniversary-in-open-source/ for a
short blog entry by Bruce Perens mentioning Electric Fence. Hayati Ayguen's
current projects are here: https://github.com/hayguen?tab=repositories

 - Jeff Johnson

--------------------
