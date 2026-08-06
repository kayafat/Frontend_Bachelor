# Prototypische Umsetzung eines MetaHuman-basierten Dozenten für interaktive Prüfungsvorbereitung in einer Unreal-Engine-Lernumgebung (Frontend)

## Projektbasis

Dieses Repository enthält das Unreal-Engine-Frontend des im Rahmen der
Bachelorarbeit entwickelten MetaHuman-basierten virtuellen Dozenten.

Das Projekt wurde mit **Unreal Engine 5.6** umgesetzt. Für die
Audio2Face-basierte Gesichtsanimation werden das **NVIDIA ACE Unreal Plugin**
sowie das lokale Modell-Plugin **NvAudio2FaceClaire** benötigt.

Das Frontend kann separat geöffnet und kompiliert werden. Für die vollständige Dialogfunktion werden zusätzlich das [Backend](https://github.com/kayafat/Backend_Bachelor.git), der Piper-Server, die Datenbankumgebung und die Ollama-Verbindung zum DACHS-Cluster benötigt.

# Installation des Frontends

## Voraussetzungen für das Frontend

Vor der erstmaligen Einrichtung werden folgende Komponenten benötigt:

- Windows 10 oder Windows 11
- [Git](https://git-scm.com/install/windows) und [Git LFS](https://git-lfs.com/)
- [Epic Games Launcher](https://store.epicgames.com/download?lang=de)
- Unreal Engine 5.6 (Download im Epic Games Launcher)
- [Visual Studio 2022](https://visualstudio.microsoft.com/vs/older-downloads/) darunter bei der Installation:
  - MSVC-v143-Buildtools
  - Windows 10 SDK oder Windows 11 SDK
  - Visual Studio Tools for Unreal Engine
- NVIDIA-Grafikkarte mit RTX-Unterstützung

Diese Vorraussetzungen werden im 4. Schritt erklärt:
- NVIDIA ACE Plugin für Unreal Engine 5.6
- lokales Audio2Face-Modell-Plugin `NvAudio2FaceClaire`

Das Projekt wurde auf folgender Hardware entwickelt und getestet:

| Komponente | Ausführung |
|---|---|
| Betriebssystem | Microsoft Windows 10 Pro |
| Prozessor | AMD Ryzen 7 7700X |
| Grafikkarte | NVIDIA GeForce RTX 4070 Ti |
| Arbeitsspeicher | 32 GB DDR5-RAM |

>[!NOTE]
>Diese Angaben stellen die verwendete Entwicklungsumgebung dar und sind nicht als verbindliche Mindestanforderungen zu verstehen.

---

## 1. Arbeitsordner anlegen und Frontend herunterladen

Öffnen Sie ein Terminal und erstellen Sie mit folgenden Befehlen einen Ordner, oder im Explorer auf dem gewünschten Laufwerk.
**Falls der Ordner schon erstellt wurde, kann dieser Schritt übersprungen werden.**
```bash
mkdir Bachelorarbeit
cd Bachelorarbeit
```
**Anschließend das Frontend-Repository im Terminal klonen:**
```
git lfs install
git clone https://github.com/kayafat/Frontend_Bachelor.git
```
**Nach dem Abschluss des Downloads folgende Befehle ausführen:**
```
cd Frontend_Bachelor
git lfs pull
```
---

## 2. Unreal Engine 5.6 installieren

Unreal Engine 5.6 wird über den Epic Games Launcher installiert:

1. Epic Games Launcher öffnen.
2. Zu **Unreal Engine → Bibliothek** wechseln.
3. Unreal Engine 5.6 installieren.
4. Den Installationspfad notieren.

<img width="607" height="343" alt="EpicGamesLauncher_lxA0dZvbsw" src="https://github.com/user-attachments/assets/b15b47f6-4142-47b7-a21e-33c656027804" />

Ein typischer Installationspfad lautet: `C:\Program Files\Epic Games\UE_5.6`. Je nach gewähltem Speicherort kann der Pfad abweichen.

---

## 3. Visual Studio 2022 einrichten

Im Visual Studio Installer die Workload `Spieleentwicklung mit C++` aktivieren. Zusätzlich sollten folgende Komponenten installiert sein:
- MSVC v143 C++ Build Tools
- Windows 10 SDK oder Windows 11 SDK
- Visual Studio Tools for Unreal Engine
- die von Unreal benötigten .NET-Komponenten

<img width="1280" height="974" alt="VirtualBoxVM_gikyPy1xke" src="https://github.com/user-attachments/assets/ac6e9e70-c328-4b0b-af1a-ec07b9679bf5" />

>[!Note]
>Nach der Installation Visual Studio und den Epic Games Launcher gegebenenfalls neu starten.

---

## 4. NVIDIA ACE und die Audio2Face Modelle installieren

Die Plugins werden vor dem Generieren der Visual-Studio-Projektdateien installiert. 
Auf der offiziellen [NVIDIA-Developer-Seite](https://developer.nvidia.com/ace-for-games) im Bereich **ACE for Games** werden folgende Pakete benötigt:

| NVIDIA ACE Unreal Plugin | Audio2Face-Modell-Plugin |
| :---: | :---: |
| <img width="510" height="326" alt="chrome_zpyINMAoBL" src="https://github.com/user-attachments/assets/a7f1150e-51ba-4a57-bbbd-73cec50bba7c" /> | <img width="499" height="261" alt="chrome_qHRTyE1xVI" src="https://github.com/user-attachments/assets/0a9c8ded-a8ff-467a-9be8-ee83079c2cd5" /> |

- **Die Daten die aus der .zip gebraucht werden sind folgende:**

| NV_ACE_Reference-UE5.6-v2.5.0rc3 | ACE_3.0_A2F_Models |
| :---: | :---: |
| <img width="317" height="161" alt="WinRAR_U7DYWHwGqQ" src="https://github.com/user-attachments/assets/ea361123-8168-459a-860b-85966e7c258b" /> | <img width="260" height="159" alt="WinRAR_AVUOH3MV05" src="https://github.com/user-attachments/assets/6542f594-4065-4072-9cbc-7c9407c212b2" /> |

Nach dem Entpacken werden insbesondere diese Pluginordner benötigt:
- NV_ACE_Reference
- NvAudio2FaceClaire
- NvAudio2FaceJames
- NvAudio2FaceMark

In den jeweiligen Ordnern muss sich direkt eine `.uplugin`-Datei befinden.

**Diese vier Ordner werden in den Marketplace-Pluginordner von Unreal Engine 5.6 kopiert:** `<UE_5.6>\Engine\Plugins\Marketplace\`.
Bei einer Standardinstallation ergibt sich beispielsweise:

<img width="507" height="161" alt="explorer_qT2eLvRgdo" src="https://github.com/user-attachments/assets/bc9a0e77-1bce-49bb-8806-597e03bfb97d" />

> [!NOTE]
> Existiert der Ordner `Marketplace` noch nicht, kann er manuell angelegt werden.
> Die Plugins dürfen nicht gleichzeitig im Engine-Ordner und im
> projektbezogenen Ordner `Frontend_Bachelor\Plugins` installiert sein.

---

## 5. Visual-Studio-Projektdateien erzeugen

Nach der Installation der Plugins:
- Rechtsklick auf `Bachelor.uproject`
- **Generate Visual Studio project files** auswählen

<img width="537" height="265" alt="generatingProjectfiles2" src="https://github.com/user-attachments/assets/30c8d091-33c4-4cac-b8d4-e45f7ccbaa2e" />

- Warten, bis die Projektdateien erfolgreich erzeugt wurden

<img width="482" height="92" alt="UnrealVersionSelector_uPPHMPtM55" src="https://github.com/user-attachments/assets/a9a667b6-ac03-41b1-a0d0-c0d6ed0d3945" />

- Danach sollte im Projektordner unter anderem `Bachelor.sln` vorhanden sein.

<img width="608" height="106" alt="sln1" src="https://github.com/user-attachments/assets/36a596cb-6afb-4ec8-9d46-7adf26ca799e" />

- Die Ordner `Binaries`, `Intermediate`, `Saved` und `.vs` werden lokal erzeugt und befinden sich deshalb nicht im GitHub-Repository.

---

## 6. Frontend kompilieren

Die Datei `Bachelor.sln` in Visual Studio 2022 öffnen.

>[!Important]
> Falls dies die erste Einrichtung ist, installieren Sie die **Desktop-Entwicklung mit C++** und die **.NET-Komponenten**:

<img width="1128" height="711" alt="VirtualBoxVM_XA9i8tzpeR" src="https://github.com/user-attachments/assets/6a9315d4-cb45-4b97-8e2f-655c2e09c939" />

- Danach **Oben in Visual Studio folgende Konfiguration auswählen:**

<img width="274" height="154" alt="devenv_OgC2ChYAQl" src="https://github.com/user-attachments/assets/ff735b77-0952-463f-9b8a-21d48d1ee360" />

- Anschließend im Projektmappen-Explorer:

<img width="503" height="233" alt="rON7dovhKO" src="https://github.com/user-attachments/assets/c3134266-5c71-4234-a912-773ee0ee63b2" />

>[!NOTE]
>Der erste Build kann mehrere Minuten dauern.
- Nach einem erfolgreichen Build sollte die Ausgabe ungefähr mit folgender Meldung enden: `Build succeeded`

>[!Important]
>Warnungen aus externen Engine- oder Plugin-Modulen sind nicht automatisch Fehler. Entscheidend ist, ob das Projekt `Bachelor` erfolgreich erstellt wurde.

---

## 7. Unreal-Engine-Projekt öffnen

Nach dem erfolgreichen Build `Bachelor.uproject` öffnen. 

<img width="621" height="109" alt="uproject1" src="https://github.com/user-attachments/assets/e2cc5c1c-632a-4724-915b-c791a2a9eec2" />

- Beim ersten Start können Shader und MetaHuman-Assets neu kompiliert werden. Dieser Vorgang kann mehrere Minuten dauern.

<img width="720" height="370" alt="UnrealEditor_95PoLi9k1W" src="https://github.com/user-attachments/assets/3d18b711-12de-4f92-80ea-12aed6e1a021" />

- **Im Unreal Editor anschließend kontrollieren:** `Bearbeiten → Plugins`

<img width="1654" height="526" alt="UnrealEditor_D9CjQfGB0R" src="https://github.com/user-attachments/assets/b7eb1a36-df28-4bfe-b8bd-5fb199fe4e6b" />

> [!NOTE]
> Falls Unreal Engine eine Aktivierung oder einen Neustart verlangt, den Editor anschließend neu starten.

---

## 8. Projekt starten

Nach dem Öffnen des Projekts:

1. Die vorgesehene Hauptmap öffnen, falls sie nicht automatisch geladen wurde.
2. Sicherstellen, dass Backend und weitere Dienste erreichbar sind.
3. Im Unreal Editor auf **Play** klicken.
<img width="166" height="60" alt="UnrealEditor_JrtQPtZmHb" src="https://github.com/user-attachments/assets/edf466e4-53b8-4116-b39f-566d3f87c1e8" />

4. Zum MetaHuman-Dozenten gehen.
5. Die im Projekt vorgesehene Interaktion auslösen.

>[!Important]
>Für die vollständige Dialogfunktion müssen parallel das [Backend](https://github.com/kayafat/Backend_Bachelor.git), Piper und das Sprachmodell verfügbar sein.

---

# Extras

## Manuelle Gestensteuerung per Tastendruck

Die Gesten des **MetaHuman-Dozenten** können zusätzlich manuell über die
Tastatur ausgelöst werden. Hierzu ist im `BP_PlayerController` jeder
verfügbaren Animation eine Taste zugeordnet.

- Ein Ausschnitt der entsprechenden Blueprint-Umsetzung ist nachfolgend
dargestellt:

<img width="665" height="493" alt="UnrealEditor_VHV8HyNKh9" src="https://github.com/user-attachments/assets/fe47d49a-2fb0-49e8-bab4-39ad2061f620" />

### Die folgenden Tasten lösen die jeweils zugeordnete Animation aus::

| Taste | Animation | Taste | Animation |
|---|---|---|---|
| Num 0 | thoughtful_head_nod | Num 7 | acknowledging_pose |
| Num 1 | thinking_pose | Num 8 | talk_pose2 |
| Num 2 | head_nod_yes | Num 9 | talk_pose3 |
| Num 3 | head_shake_no | Num ÷ | arm_gesture |
| Num 4 | pointing_pose | Num × | pointing_forward |
| Num 5 | talk_pose | Einfg | hello_pose |
| Num 6 | surprised_pose | Entf | bye_pose |

---


# Problembehandlung im Frontend

### `Not in a Git repository`
- Der Befehl wurde außerhalb des Repository-Ordners ausgeführt.
```bat
...\Bachelorarbeit\Frontend_Bachelor
git lfs pull
```

---


### Unreal-Assets sind ungewöhnlich klein oder können nicht geöffnet werden
- Git LFS wurde nicht vollständig ausgeführt:
```bat
git lfs install
git lfs pull
```

---

### Benötigtes Plugin fehlt
- Prüfen, ob folgende Dateien vorhanden sind:
```text
<UE_5.6>\Engine\Plugins\Marketplace\NV_ACE_Reference\NV_ACE_Reference.uplugin
<UE_5.6>\Engine\Plugins\Marketplace\NvAudio2FaceClaire\NvAudio2FaceClaire.uplugin
<UE_5.6>\Engine\Plugins\Marketplace\NvAudio2FaceJames\NvAudio2FaceJames.uplugin
<UE_5.6>\Engine\Plugins\Marketplace\NvAudio2FaceMark\NvAudio2FaceMark.uplugin
```

---

### `Bachelor.sln` fehlt
- Visual-Studio-Projektdateien erneut erzeugen:
- Rechtsklick auf `Bachelor.uproject` → Generate Visual Studio project files


---

### Unreal meldet fehlende oder veraltete Module
- `Bachelor.sln` öffnen und das Projekt mit folgender Konfiguration neu bauen:
```text
Development Editor
x64
```

---

### Build in `Bachelor.sln` kann nicht erstellt werden → *Result: Failed (OtherCompilationError)*

<img width="2129" height="610" alt="mspaint_H8I3dnrCUy" src="https://github.com/user-attachments/assets/565987a3-3e70-4272-8530-7da515f3dc60" />

- Stellen Sie sicher dass **NV_ACE_Reference** und die Audio2Face-Modelle sich im Marketplace Ordner befinden. **Überprüfen Sie erneut den Schritt 4.**

---

### CUDA Fehler?

Wenn Fehler bezüglich CUDA auftreten sollten: [NVIDIA GPU Computing Toolkit](https://developer.nvidia.com/cuda-toolkit-archive) Version `CUDA v12.8.1` zusätzlich installieren.

---

### Das geöffnete Projekt zeigt nur eine schwarze Welt. Die Hauptmap wurde nicht geladen?

- Wählen Sie die **map_1** aus, um die virtuelle Lernumgebung und den MetaHuman zu sehen.

<img width="1598" height="683" alt="UnrealEditor_fC1hkc6Fwb" src="https://github.com/user-attachments/assets/54fc14ac-bf4a-44eb-b688-82c603762854" />

---

### Die Ausführung des Projekts überlastet meinen Computer

- Sie können das Viewport im folgenden anpassen:

<img width="870" height="641" alt="oqmys4SSyF" src="https://github.com/user-attachments/assets/b6b3b709-2d18-47fd-96ed-96596bedff6f" />

---

>### Autor
>- **Fatih Kaya**
>- Bachelorarbeit, Hochschule Esslingen
