# DUMA

[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://github.com/johnsonjh/duma/blob/master/COPYING-GPL)
[![License: LGPL v2.1](https://img.shields.io/badge/License-LGPL%20v2.1-blue.svg)](https://github.com/johnsonjh/duma/blob/master/COPYING-LGPL)
[![FOSSAStatus](https://app.fossa.com/api/projects/git%2Bgithub.com%2Fjohnsonjh%2Fduma.svg?type=shield)](https://app.fossa.com/projects/git%2Bgithub.com%2Fjohnsonjh%2Fduma?ref=badge_shield)
[![LocCount](https://img.shields.io/tokei/lines/github/johnsonjh/duma.svg)](https://github.com/XAMPPRocky/tokei)
[![GitHubCodeSize](https://img.shields.io/github/languages/code-size/johnsonjh/OldCurve25519ScalarMult.svg)](https://github.com/johnsonjh/OldCurve25519ScalarMult)
[![LgtmAlerts](https://img.shields.io/lgtm/alerts/g/johnsonjh/duma.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/johnsonjh/duma/alerts/)
[![CodacyBadge](https://api.codacy.com/project/badge/Grade/f777934d666b4a6a9672d89b404c4953)](https://app.codacy.com/gh/johnsonjh/duma?utm_source=github.com&utm_medium=referral&utm_content=johnsonjh/duma&utm_campaign=Badge_Grade)
[![CodeBeat](https://codebeat.co/badges/a0be6809-acda-41a7-96a8-0d46698dc42c)](https://codebeat.co/projects/github-com-johnsonjh-duma-master)
[![TickgitTODOs](https://img.shields.io/endpoint?url=https://api.tickgit.com/badge?repo=github.com/johnsonjh/duma)](https://www.tickgit.com/browse?repo=github.com/johnsonjh/duma)

## Detect Unintended Memory Access

### A Red-Zone memory allocator

#### Introduction

Before starting this project, it was clear that the antiquated `CVS` version
control system would be a limiting factor to any revived DUMA distribution.
If the project was going to survive in the long-term, it would be necessary
to perform a conversion to a more modern, common, and accessible system.

Unaware of of the [original author](https://github.com/hayguen/duma) having
an existing repository based on conversion of the legacy `CVS` repository to
`Git`, I created this one completely independently, with much effort put
forth to identify the authors of many patches and merging the many community
contributions that had accumulated over the years. I am mostly satisfied with
the end result, especially considering that many of my independent
conclusions are close or identical to those in the author's own conversion
effort. I have no special knowledge of this software, besides being an
end-user, and had no relationship or previous formal contact with the author.

The largest part of the conversion was tracking down the many changes and
patches, determining who actually wrote them and when, and then determining
if these changes should be incorporated into the new "mainline" repository.

This required many "educated guesses", and, when lacking needed details, pure
judgment calls. With such an old, widely distributed, hacked-upon code-base,
surviving as long as it did, without a strongly maintained official home or
new releases for many years, much of data available is fragmentary or
conflicting. Sometimes the only sources of information are the (fading)
memories of those originally involved with project. Sometimes, inferences
would need to be made, based on often inaccurate information.

Because of such uncertainties, I am absolutely certain that mistakes have
been made and errors introduced, so I welcome any contributions from the
public that would help to make the repository more historically accurate.

#### The Future of DUMA

A large task which is not yet complete is reconciling this primary
reconstruction against the last version which was released by the
[original author](https://github.com/hayguen/duma), focusing on identifying
(and correcting) any significant deviations, especially in the
identifications of contributors, as well as ensuring any disputes that might
arise are fairly handled. Any new discoveries will into the making the commit
logs for the project accurate for posterity.

The structure of my conversion very closely resembles that of the original
`CVS` repository, where the author's effort included a radical refactoring
of the directory structure. I believe that his was the more prudent choice,
and eventually will include similar changes in this repository.

This repository may be a bit pedantic, especially when it comes to including
and properly acknowledging the authors of many patches that were found on
the original SourceForge site, random forums, in other repositories, and on
many mailing lists archives. In other ways, however, this version may be
considered quite "liberal", perhaps even a distinct fork, since I have
included my own changes to the packaging, and extended the build system in
ways distinct from the original work.

#### What is the "official" upstream?

At this time, it's *very* likely that **this** repository will be
the new "official" home for D.U.M.A. releases.

Hayati Ayguen, the original author, is no longer actively maintaining
D.U.M.A. - he's moved on to other projects. While he was happy to correspond
with me, it was obvious that he wouldn't want to deal with ongoing burden of
continued maintenance and support requests that would be necessary if he were
to continue as the primary maintainer of this tool.

#### Conclusion

D.U.M.A. started out as a fork of *Bruce Perens'* venerable "`Electric Fence`",
a tool created at "Pixar", way back in 1987. `D.U.M.A.` was an enhanced fork
of this software, known to be working on Linux systems as early as 1992.

Now, nearly 30 years later, we should thank Hayati Ayguen, and all the other
contributors who put time and effort into this project, but allow them move
on to other things if they wish. They can do so knowing D.U.M.A. is not gone
or forgotten, and that it will be properly maintained as need be. As is
customary in the world of open source software, I have extended an "official"
and permanently open-ended invitation to the original author, should he decide
to resume the role of primary maintainer. Similar invitations have been sent
to all the original contributors I could track down as well.

See <https://perens.com/2017/04/04/my-30th-anniversary-in-open-source/>
for a short blog entry by Mr. Bruce Perens' mentioning `Electric Fence`.

Hayati Ayguen's current projects are available at <https://github.com/hayguen>

## License

[![FOSSA Status](https://app.fossa.com/api/projects/git%2Bgithub.com%2Fjohnsonjh%2Fduma.svg?type=large)](https://app.fossa.com/projects/git%2Bgithub.com%2Fjohnsonjh%2Fduma?ref=badge_large)
