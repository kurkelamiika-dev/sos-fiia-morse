# Development Workflow

Tämä dokumentti määrittelee projektin kehitystehtävien, GitHub Issues -työlistan, AI-avusteisen suunnittelun, muutosten hyväksynnän, testauksen ja Git-toimintojen yhteisen toimintamallin.

Tavoitteena on, että jokainen muutos on:

- rajattu
- jäljitettävä
- hyväksytty ennen toteutusta
- testattu tarkoituksenmukaisesti
- yhdistettävissä oikeaan GitHub Issueen
- turvallisesti peruttavissa

## 1. Perusperiaate

Kehitystyö etenee oletuksena tässä järjestyksessä:

1. Lue repository muuttamatta mitään.
2. Vahvista työhakemisto ja `git status`.
3. Kuvaa nykytila.
4. Rajaa aktiivinen tehtävä.
5. Tarkista mahdolliset siihen liittyvät avoimet issuet.
6. Ehdota pienintä turvallista muutosta.
7. Näytä suunniteltu diff tai täsmällinen toimenpide.
8. Pyydä hyväksyntä.
9. Tee vain hyväksytty muutos.
10. Aja pienin relevantti testi.
11. Näytä `git diff` ja `git status`.
12. Tee commit vain erillisellä luvalla.
13. Tee push vain erillisellä luvalla.

## 2. GitHub Issues -käyttötapa

GitHub Issue toimii yhden rajatun kehitystehtävän, virheen, testitarpeen, dokumentointityön tai idean työmäärityksenä.

Yksi issue saa sisältää vain sellaisen kokonaisuuden, joka voidaan:

- ymmärtää itsenäisesti
- hyväksyä tai hylätä itsenäisesti
- testata selkeillä hyväksymiskriteereillä
- sulkea ilman, että muita erillisiä tehtäviä jää piiloon

Älä yhdistä samaan issueen useita toisistaan riippumattomia ominaisuuksia.

## 3. Suositellut labelit

Projektissa voidaan käyttää esimerkiksi seuraavia labeleita.

### Priority

- `priority:critical`
- `priority:high`
- `priority:medium`
- `priority:low`

### Type

- `type:feature`
- `type:bug`
- `type:refactor`
- `type:test`
- `type:documentation`
- `type:research`

### Area

- `area:firmware`
- `area:serial`
- `area:morse`
- `area:wifi`
- `area:mqtt`
- `area:sensors`
- `area:workbench`
- `area:tests`
- `area:ci`
- `area:documentation`

### Effort

- `effort:small`
- `effort:medium`
- `effort:large`

### Status

- `status:idea`
- `status:ready`
- `status:blocked`
- `status:in-progress`
- `status:review`

## 4. Nopea copy-paste Issue -pohja

Kopioi alla oleva pohja uuteen GitHub Issueen. Poista kaikki epärelevantit otsikot, vaihtoehdot, ohjetekstit ja tyhjiksi jäävät kohdat ennen tallentamista.

```markdown
## Goal

[What should be achieved?]

## Background

[Why is this needed?]

## Current behavior

[What happens now? Remove if not relevant.]

## Expected behavior

[What should happen after the change?]

## Scope

Included:
- [Required change]
- [Required test or documentation]

Not included:
- [Explicitly excluded change]
- [Future improvement that belongs to another issue]

## Acceptance criteria

- [ ] [Observable result 1]
- [ ] [Observable result 2]
- [ ] [Relevant existing behavior still works]
- [ ] [Required test passes]
- [ ] [Documentation is updated, if needed]

## Relevant files or components

- `[path/to/file]`
- `[component or subsystem]`

## Target environment

- Board: `[exact board or UNKNOWN]`
- Chip: `[chip family or UNKNOWN]`
- PlatformIO environment: `[env name or UNKNOWN]`
- Workbench slot: `[slot label or NOT APPLICABLE]`
- Serial baud: `[baud rate or UNKNOWN]`

## Test plan

- [ ] Read-only repository analysis
- [ ] Build only
- [ ] Flash to device
- [ ] Serial output verification
- [ ] Hardware-in-the-loop test
- [ ] Unit test
- [ ] Manual verification

Exact command, if known:

```bash
[command]
```

## Risks and constraints

- [Do not change Wi-Fi credentials]
- [Do not erase flash]
- [Do not modify unrelated files]
- [Other project-specific constraint]

## Related issues

- Related to #[issue number]
- Blocked by #[issue number]
- Follow-up: #[issue number]

## Definition of done

- [ ] Approved scope has been implemented
- [ ] Relevant test evidence has been recorded
- [ ] `git diff` contains no unrelated changes
- [ ] Commit references this issue
- [ ] Issue can be closed without hidden follow-up work
```

## 5. Esimerkki valmiiksi täytetystä lyhyestä Issuesta

```markdown
# Add serial status output for Morse firmware

## Goal

Print the active Morse message and execution state to the serial monitor.

## Background

The onboard LED already sends the Morse message, but the user currently has no textual confirmation of the message or current state.

## Current behavior

The LED flashes the message, but serial output does not show when transmission starts, what message is being sent, or when transmission ends.

## Expected behavior

The serial monitor shows:

- firmware startup
- active Morse message
- transmission start
- transmission completion
- relevant error state, if transmission cannot start

## Scope

Included:
- Add minimal serial logging to the existing Morse execution path.
- Preserve the current LED timing and message.
- Verify output using the physical ESP32 serial connection.

Not included:
- Changing Morse timing.
- Adding Wi-Fi, MQTT, sensors, or Workbench API features.
- Refactoring the whole Morse implementation.

## Acceptance criteria

- [ ] Serial output shows the exact active Morse message.
- [ ] Serial output shows when transmission starts.
- [ ] Serial output shows when transmission completes.
- [ ] Existing LED Morse behavior remains unchanged.
- [ ] The firmware builds successfully.
- [ ] Output is verified on the physical ESP32.

## Relevant files or components

- `src/main.cpp`
- Existing Morse transmission logic
- Serial monitor

## Target environment

- Board: use the value from `platformio.ini`
- Chip: verify before flashing
- PlatformIO environment: use the existing environment from `platformio.ini`
- Workbench slot: not required for the first physical serial verification
- Serial baud: use `monitor_speed` from `platformio.ini`

## Test plan

- [ ] Read `platformio.ini` and `src/main.cpp` without modifying files.
- [ ] Show the planned diff.
- [ ] Build after separate approval.
- [ ] Flash after separate approval.
- [ ] Open the serial monitor and verify the expected lines.
- [ ] Show `git diff` and `git status`.

## Risks and constraints

- Do not change Morse timing.
- Do not change the active Morse message.
- Do not change board, port, or baud settings without verified repository evidence.
- Do not commit or push without separate approval.

## Definition of done

- [ ] Required serial messages are visible.
- [ ] Existing LED behavior still works.
- [ ] No unrelated files are changed.
- [ ] Test evidence is recorded in the issue or development log.
```

## 6. Pitkä kommentoitu Issue -malli

Alla oleva malli sisältää HTML-kommentteja. GitHub ei näytä kommentteja valmiin issuen normaalissa näkymässä, mutta ne näkyvät muokkaustilassa. Poista silti tarpeettomat osiot ja vaihtoehdot ennen issuen tallentamista.

```markdown
<!--
ISSUE TITLE

Kirjoita otsikko toimintamuodossa ja niin täsmällisesti, että tehtävä ymmärretään ilman issue-listan avaamista.

Hyvä:
Add MQTT connection status to Morse indicator

Huono:
MQTT improvement
-->

# [Action-oriented issue title]

## Goal

<!--
Kerro yhdellä tai kahdella lauseella, mikä lopputulos halutaan.

Tämän kohdan pitää vastata kysymykseen:
"Mitä käyttäjä tai järjestelmä pystyy tekemään tämän issuen valmistuttua?"

Älä kuvaa tässä toteutustapaa liian aikaisin.
-->

[Describe the required end result.]

## Background

<!--
Kerro, miksi tehtävä on tarpeellinen.

Lisää vain sellaista taustaa, joka auttaa tekijää tekemään oikeita ratkaisuja.
Voit mainita esimerkiksi:
- nykyisen käyttötapauksen
- havaitun ongelman
- riippuvuuden tulevasta ominaisuudesta
- miksi työ kannattaa tehdä juuri nyt

Älä lisää pitkää projektihistoriaa, ellei sillä ole teknistä merkitystä.
-->

[Explain why this work is needed.]

## Evidence or observed behavior

<!--
Täytä tämä erityisesti bugissa tai epäselvässä nykytilassa.

Kirjaa vain havaittu tieto:
- tarkka lokirivi
- serial output
- virheilmoitus
- toistettava fyysinen käyttäytyminen
- kuvakaappauksen tai videon viite

Älä sekoita havaintoon omaa hypoteesia.
Poista koko osio, jos kyseessä on puhdas uusi ominaisuus eikä nykytilan evidenssiä tarvita.
-->

Observed:

```text
[Exact log, serial output, or error excerpt]
```

Consistency:

- [ ] Always reproducible
- [ ] Intermittent
- [ ] Observed once
- [ ] Not yet verified

## Current behavior

<!--
Kuvaa, mitä järjestelmä tekee nyt.

Käytä havaittavaa toimintaa, älä epämääräisiä arvioita.
Esimerkiksi:
"Serial monitor remains empty after reset."

Poista osio, jos nykykäyttäytyminen ei ole olennainen.
-->

[Describe the current observable behavior.]

## Expected behavior

<!--
Kuvaa, mitä järjestelmän pitäisi tehdä muutoksen jälkeen.

Tee tästä niin täsmällinen, että acceptance criteria voidaan johtaa siitä.
-->

[Describe the expected observable behavior.]

## Scope

<!--
Tämä on yksi tärkeimmistä kohdista.

"Included" kertoo, mitä tässä issuessa saa toteuttaa.
"Not included" estää scope creepin ja kirjaa tiedossa olevat mutta myöhemmäksi jätettävät asiat.

Jos Claude tai ChatGPT löytää läheisen kehitystehtävän, sitä ei lisätä tähän automaattisesti. Se ehdotetaan erikseen ja käyttäjä hyväksyy mahdollisen scope-laajennuksen.
-->

Included:

- [Exact required change 1]
- [Exact required change 2]
- [Required test or documentation]

Not included:

- [Related but separate feature]
- [Large refactor not required for this result]
- [Persistent configuration change]
- [Any unrelated cleanup]

## Proposed approach

<!--
Täytä tämä vain, jos toteutustavasta on jo perusteltu käsitys.

Erota varmistettu tieto ja ehdotus.
Älä lukitse toteutusta tarpeettomasti, jos repository-analyysi on vielä tekemättä.

Voit käyttää muotoa:
Verified from repository:
- ...

Proposed:
- ...

Open question:
- ...
-->

Verified from repository:

- [Confirmed architecture, file, function, test path, or build setting]

Proposed:

- [Smallest safe implementation approach]

Open questions:

- [Unverified target, board, port, API behavior, or design choice]

## Acceptance criteria

<!--
Kirjoita jokainen kohta havaittavana PASS/FAIL-ehtona.

Hyvä:
- Serial output contains `MQTT CONNECTED` within 5 seconds after successful connection.

Huono:
- MQTT handling is improved.

Lisää myös regressiosuoja eli tärkeä nykyinen toiminta, jonka pitää edelleen toimia.
-->

- [ ] [Observable functional result]
- [ ] [Observable error or fallback behavior]
- [ ] [Existing relevant behavior still works]
- [ ] [Build or test command succeeds]
- [ ] [Required serial, API, or hardware evidence is recorded]
- [ ] [Documentation is updated if user-visible behavior changes]

## Relevant files and components

<!--
Luettele vain tunnetut tai todennäköisesti liittyvät tiedostot ja komponentit.

Älä väitä tiedostoa relevantiksi, jos repositorya ei ole vielä luettu. Käytä tarvittaessa merkintää "to be verified".
-->

Known:

- `[path/to/file]`
- `[function, class, component, API endpoint, or test]`

To be verified:

- `[possible path or subsystem]`

## Target environment

<!--
Täytä arvot repositoryn, Workbench devices-discoveryn tai fyysisen tarkistuksen perusteella.
Älä kopioi arvoja aiemmasta projektista ilman vahvistusta.

Käytä UNKNOWN, jos tietoa ei vielä ole.
-->

- Repository: `[repository URL or local path]`
- Branch: `[branch name]`
- Commit before work: `[commit hash or UNKNOWN]`
- Build system: `[PlatformIO / ESP-IDF / other]`
- PlatformIO environment: `[environment name or UNKNOWN]`
- Board: `[exact board model or UNKNOWN]`
- PlatformIO board ID: `[board ID or UNKNOWN]`
- Chip family: `[chip family or UNKNOWN]`
- Framework: `[Arduino / ESP-IDF / other / UNKNOWN]`
- Upload protocol: `[protocol or UNKNOWN]`
- Upload port: `[port or AUTO / UNKNOWN]`
- Monitor port: `[port or AUTO / UNKNOWN]`
- Serial baud: `[baud or UNKNOWN]`
- Workbench host: `[WORKBENCH_HOST or NOT APPLICABLE]`
- Workbench slot: `[WORKBENCH_SLOT or NOT APPLICABLE]`
- Workbench devnode: `[devnode or UNKNOWN]`

## Approval gates

<!--
Merkitse kaikki tässä issuessa mahdollisesti tarvittavat erilliset hyväksynnät.
AI-agentti ei saa yhdistää näitä yhdeksi yleisluvaksi ilman käyttäjän nimenomaista pyyntöä.
-->

- [ ] File modification approval
- [ ] Build approval
- [ ] Flash approval
- [ ] Hardware-in-the-loop test approval
- [ ] Network or Workbench state-changing API approval
- [ ] Git commit approval
- [ ] Git push approval

## Test plan

<!--
Kerro ensin, mitä kukin testi oikeasti tekee.
Valitse pienin relevantti verifiointi.
Poista epärelevantit vaihtoehdot.

Älä merkitse testiä suoritetuksi ennen todellista PASS-näyttöä.
-->

### Read-only checks

```bash
pwd
git status --short
git diff --stat
[repository-specific read-only commands]
```

Expected evidence:

- [Correct working directory]
- [Known modified files]
- [Verified target configuration]

### Build

<!-- Poista tämä alakohta, jos buildia ei tarvita. -->

Command:

```bash
[exact build command]
```

Expected result:

- [Exact success indicator or artifact path]

### Flash

<!--
Poista tämä alakohta, jos flashausta ei tarvita.
Ennen flashausta varmista artifact, environment, slot, chip ja reset behavior.
-->

Artifact:

```text
[exact firmware artifact path]
```

Target:

```text
Environment: [env]
Slot: [slot]
Chip: [chip]
Port or devnode: [port]
```

Flash action:

```bash
[exact command or API action]
```

Expected reset behavior:

- [Expected LED, boot log, serial output, or device state]

### Serial verification

<!-- Poista tämä alakohta, jos serial output ei kuulu testiin. -->

Command:

```bash
[exact serial monitor command]
```

Expected patterns:

```text
[expected line 1]
[expected line 2]
```

### Hardware-in-the-loop test

<!-- Poista tämä alakohta, jos HIL-testiä ei tarvita. -->

Command:

```bash
[exact pytest or project test command]
```

This test may:

- build firmware
- flash the target
- reset the target
- use Workbench resources
- read serial output
- change temporary lab state

Expected PASS evidence:

```text
[exact PASS indicator]
```

### Manual verification

<!--
Kuvaa fyysisesti tarkastettava toiminta yksiselitteisesti.
Esimerkiksi LED-jakso, releen tila, sensorin arvo tai mobiilisovelluksen näyttämä tulos.
-->

- [ ] [Physical observation 1]
- [ ] [Physical observation 2]

## Risks

<!--
Kirjaa realistiset riskit, jotka voivat vaikuttaa turvallisuuteen, laitteeseen, dataan tai työn rajaukseen.

Esimerkkejä:
- wrong target device
- persistent Wi-Fi configuration change
- flash erase
- timing regression
- unrelated staged files
- Workbench API exposed outside trusted network
-->

- [Risk 1]
- [Risk 2]

## Constraints

<!--
Kirjaa ehdottomat rajat.
Poista oletusesimerkit, jotka eivät koske tehtävää.
-->

- Do not use `erase_flash`.
- Do not perform a full-chip flash.
- Do not change persistent Wi-Fi, OTA, or device credentials.
- Do not modify unrelated files.
- Do not stage unrelated files.
- Do not commit without explicit approval.
- Do not push without explicit approval.
- [Additional task-specific constraint]

## Rollback plan

<!--
Kerro, miten palataan turvallisesti lähtötilaan.

Pienessä lähdekoodimuutoksessa tämä voi olla diffistä palautus ennen commitia.
Firmware-muutoksessa rollback voi vaatia aiemman tunnetun artefaktin uudelleenflashauksen.
Älä lupaa rollbackia, jota ei ole oikeasti valmisteltu.
-->

Before commit:

- Restore only the files changed for this issue.
- Verify with `git diff` and `git status`.

After commit but before push:

- Use a new corrective commit unless history rewriting has been explicitly approved.

Device rollback:

- [Previous known-good artifact or exact recovery procedure]

## Related issues and nearby backlog tasks

<!--
Lisää tähän vain todelliset liittyvät issuet.

Jos AI havaitsee läheisen tehtävän, sen pitää raportoida:
- issue number and title
- overlap reason
- added effort
- added risk
- recommendation: now or separately

Älä muuta tämän issuen scopea ilman käyttäjän hyväksyntää.
-->

- Related to #[issue number]: [reason]
- Blocked by #[issue number]: [reason]
- Blocks #[issue number]: [reason]
- Suggested follow-up #[issue number]: [reason]

## Implementation log

<!--
Täytetään työn edetessä.
Pidä merkinnät lyhyinä ja todennettavina.
Älä kirjoita "fixed" tai "passed", ellei siitä ole näyttöä.
-->

### Read-only analysis

- Working directory: `[path]`
- Initial branch: `[branch]`
- Initial commit: `[hash]`
- Initial git status: `[clean / modified files]`
- Verified files: `[paths]`

### Approved change

- Approval received for: `[exact scope]`
- Planned diff: `[short description]`

### Change result

- Changed files: `[paths]`
- Actual result: `[what changed]`
- Unexpected result: `[none or description]`

### Test evidence

- Command: `[command]`
- Result: `[PASS / FAIL / NOT RUN]`
- Relevant output:

```text
[short exact excerpt]
```

### Git review

- `git diff --stat`: `[summary]`
- `git status --short`: `[summary]`
- Unrelated changes present: `[yes/no]`

### Commit and push

- Commit approved: `[yes/no]`
- Commit hash: `[hash or NOT CREATED]`
- Commit message: `[message or NOT CREATED]`
- Push approved: `[yes/no]`
- Remote branch: `[remote/branch or NOT PUSHED]`

## Definition of done

<!--
Issue voidaan sulkea vasta, kun kaikki tähän tehtävään kuuluvat olennaiset ehdot täyttyvät.

Jos jokin uusi tehtävä löytyy työn aikana, tee siitä erillinen issue äläkä piilota sitä tämän alle.
-->

- [ ] Approved scope is fully implemented.
- [ ] Every acceptance criterion has verifiable evidence.
- [ ] Relevant tests have passed or an explicit exception is documented.
- [ ] No unrelated changes are included.
- [ ] `git diff` and `git status` have been reviewed.
- [ ] Documentation has been updated where required.
- [ ] Commit references this issue.
- [ ] Follow-up work has separate issues.
- [ ] The issue can be closed without hidden unfinished work.
```

## 7. Läheisten backlog-tehtävien käsittely

Kun aktiivista tehtävää suunnitellaan ja GitHub Issues -lista on käytettävissä, Claude tai muu AI-agentti voi tunnistaa läheisiä tehtäviä.

Läheinen tehtävä voi liittyä esimerkiksi:

- samaan tiedostoon
- samaan komponenttiin
- samaan testipolkuun
- samaan serial- tai API-rajapintaan
- samaan arkkitehtuuriratkaisuun
- muutokseen, joka olisi myöhemmin vaikeampi tehdä erikseen

AI-agentti ei saa toteuttaa läheistä tehtävää automaattisesti.

Sen pitää raportoida esimerkiksi näin:

```text
Nykyiseen tehtävään liittyy avoin issue #18: Add unit tests for Morse encoding.

Overlap:
- Molemmat tehtävät koskevat Morse-enkoodauslogiikkaa.
- Testi olisi helpompi lisätä, jos logiikka erotetaan samalla puhtaaksi funktioksi.

Estimated additional scope:
- 1 new test file
- small refactor in the existing Morse implementation

Additional risks:
- Refactor may change timing behavior if not kept separate from LED scheduling.

Recommendation:
- Do not include automatically.
- Consider approving it as an explicit scope extension before file changes.
```

## 8. Commit-viestit ja Issue-linkitys

Suositeltu commit-muoto:

```text
Add serial status output for Morse transmission

Relates to #12
```

Käytä `Closes #12` tai `Fixes #12` vain, kun commit tai siihen kuuluva yhdistetty muutos todella täyttää koko issuen Definition of done -ehdot.

Esimerkki:

```text
Add serial status output for Morse transmission

Closes #12
```

Älä käytä sulkevaa viittausta, jos:

- vain osa acceptance criteria -kohdista valmistui
- testi puuttuu
- dokumentointi puuttuu
- tunnettu jatkotyö kuuluu yhä samaan issueen

## 9. AI-agentille annettava tehtäväpyyntö

Seuraavaa pyyntöä voidaan käyttää repository-analyysin alussa:

```text
Lue tämä repository muuttamatta tiedostoja.

Näytä ensin:
- pwd
- git status
- nykyinen branch ja commit
- platformio.ini tai muu build-konfiguraatio
- relevantit source-, test- ja docs-tiedostot

Selitä:
- mitä firmware tekee nyt
- miten build toimii
- miten flashing toimii
- miten testit toimivat
- mitä tämä issue edellyttää
- löytyykö avoimista GitHub Issues -tehtävistä nykyiseen tehtävään läheisesti liittyviä töitä

Jos löydät läheisen tehtävän, raportoi issue-numero, overlap, lisätyö, riski ja suositus. Älä laajenna scopea.

Älä muokkaa tiedostoja, aja buildia, flashaa, aja testejä, tee commitia tai pushia.
```

## 10. Dokumentin ylläpito

Päivitä tätä dokumenttia, kun projektin todellinen työnkulku muuttuu.

Älä lisää tähän oletuksia, jotka koskevat vain yhtä laitetta tai yhtä väliaikaista testiä. Laitteeseen, boardiin, slottiin, porttiin ja ympäristöön liittyvät arvot kuuluvat ensisijaisesti projektin omaan konfiguraatioon, issueen tai testidokumentaatioon.
