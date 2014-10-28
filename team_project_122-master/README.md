##CSCI 104 Team Project Repository

###Student Information
**Developer 1**:
  + Name: Trevor Cai
  + USC Email: tcai@usc.edu
  + USC ID: 4338012984

**Developer 2**:
  + Name: Carter Rawlings
  + USC Email: crawling@usc.edu
  + USC ID: 7744678801

To compile and run crawler:
`make -f mk_crawler`
`./bin/crawler SEEDFILE OUTPUTFILE`

To compile and run application:
`qmake`
`make`
`./bin/team_project_122 INDEXFILE ADSFILE OUTPUTFILE`

Do NOT run `qmake -project`; we have customized our .pro file.

Part 2 is users - after starting the search engine, (multiple) users can be created, each of whom has a history.
Any pages visited from the search menu are recorded and given a weighting for future searches.
