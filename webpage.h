// Contenu pour webpage.h

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fr" data-bs-theme="dark">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Config Macropad</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/css/bootstrap.min.css" rel="stylesheet">
    <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap-icons@1.11.1/font/bootstrap-icons.css">
    <style>
        body { padding-top: 70px; padding-bottom: 60px; }
        .macro-item { background-color: var(--bs-secondary-bg); padding: 1rem; border-radius: var(--bs-border-radius); }
        footer { font-size: 0.8em; color: var(--bs-secondary-color); }
        #iconList li { display: flex; align-items: center; gap: 10px; }
        #iconList img { background-color: #fff; }
    </style>
</head>
<body>
    <header class="navbar navbar-expand-lg fixed-top bg-body-tertiary">
        <div class="container-fluid">
            <a class="navbar-brand" href="#">Configuration Macropad</a>
            <div class="d-flex align-items-center">
                <button class="btn btn-outline-secondary me-3" data-bs-toggle="modal" data-bs-target="#helpModal"><i class="bi bi-question-circle"></i></button>
                <div class="form-check form-switch">
                    <input class="form-check-input" type="checkbox" role="switch" id="theme-switch">
                    <label class="form-check-label" for="theme-switch"><i class="bi bi-sun-fill"></i></label>
                </div>
            </div>
        </div>
    </header>

    <main class="container my-4">
        <ul class="nav nav-tabs" id="layerTab" role="tablist">
            <li class="nav-item" role="presentation">
                <button class="nav-link active" id="layer0-tab" data-bs-toggle="tab" data-bs-target="#layer0-pane" type="button" role="tab">Layer 0</button>
            </li>
            <li class="nav-item" role="presentation">
                <button class="nav-link" id="layer1-tab" data-bs-toggle="tab" data-bs-target="#layer1-pane" type="button" role="tab">Layer 1</button>
            </li>
            <li class="nav-item" role="presentation">
                <button class="nav-link" id="layer2-tab" data-bs-toggle="tab" data-bs-target="#layer2-pane" type="button" role="tab">Layer 2</button>
            </li>
        </ul>
        <div class="tab-content pt-3" id="layerTabContent">
            <div class="tab-pane fade show active" id="layer0-pane" role="tabpanel"></div>
            <div class="tab-pane fade" id="layer1-pane" role="tabpanel"></div>
            <div class="tab-pane fade" id="layer2-pane" role="tabpanel"></div>
        </div>

        <button class="btn btn-primary w-100 mt-4" onclick="saveConfig()">Sauvegarder les modifications</button>
    </main>

    <div class="modal fade" id="helpModal" tabindex="-1">
      <div class="modal-dialog modal-dialog-scrollable">
        <div class="modal-content">
          <div class="modal-header">
            <h5 class="modal-title">Icônes Disponibles</h5>
            <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
          </div>
          <div class="modal-body">
            <p>Voici la liste des noms d'icônes que vous pouvez copier/coller :</p>
            <ul class="list-group" id="iconList"></ul>
          </div>
        </div>
      </div>
    </div>

    <div class="modal fade" id="saveStatusModal" tabindex="-1">
      <div class="modal-dialog modal-dialog-centered">
        <div class="modal-content">
          <div class="modal-body text-center fs-5 p-4">
            <p id="saveStatusMessage"></p>
          </div>
        </div>
      </div>
    </div>

    <footer class="fixed-bottom bg-body-tertiary d-flex justify-content-between align-items-center px-3 py-2">
        <small>ESP32 IP: %IP_ADDRESS% | Auteur: %AUTHOR% | Firmware: %FIRMWARE_VERSION%</small>
        <small id="datetime"></small>
    </footer>

    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/js/bootstrap.bundle.min.js"></script>
    <script>
        const NUM_LAYERS = 3;
        const NUM_KEYS = 9;

        // --- Logique du Thème ---
        const themeSwitch = document.getElementById('theme-switch');
        const htmlEl = document.documentElement;
        const currentTheme = localStorage.getItem('theme') || 'dark';
        htmlEl.setAttribute('data-bs-theme', currentTheme);
        if (currentTheme === 'light') themeSwitch.checked = true;
        themeSwitch.addEventListener('change', () => {
            const newTheme = themeSwitch.checked ? 'light' : 'dark';
            htmlEl.setAttribute('data-bs-theme', newTheme);
            localStorage.setItem('theme', newTheme);
        });

        // --- Logique de chargement des données ---
        window.onload = () => {
            fetch('/config')
                .then(response => response.json())
                .then(data => {
                    for (let l = 0; l < NUM_LAYERS; l++) {
                        const pane = document.getElementById(`layer${l}-pane`);
                        let gridHtml = `<div class="row g-3">`;
                        for (let k = 1; k <= NUM_KEYS; k++) {
                            const keyId = `L${l}K${k}`;
                            const macro = data[keyId] || { cmd: '', label: '', icon: '' };
                            gridHtml += `
                                <div class="col-md-4">
                                    <div class="macro-item">
                                        <label for="${keyId}_cmd" class="form-label fw-bold">Touche ${k}</label>
                                        <input type="text" class="form-control" id="${keyId}_cmd" placeholder="Commande..." value="${macro.cmd}">
                                        <input type="text" class="form-control mt-2" id="${keyId}_label" placeholder="Libellé..." value="${macro.label}">
                                        <input type="text" class="form-control mt-2" id="${keyId}_icon" placeholder="Icône..." value="${macro.icon}">
                                    </div>
                                </div>
                            `;
                        }
                        gridHtml += `</div>`;
                        pane.innerHTML = gridHtml;
                    }
                });
        };

        // --- Logique de la fenêtre d'aide (Modal) ---
        const helpModalEl = document.getElementById('helpModal');
        helpModalEl.addEventListener('show.bs.modal', () => {
            const iconList = document.getElementById('iconList');
            iconList.innerHTML = '<li class="list-group-item">Chargement...</li>';
            fetch('/getIconList')
                .then(response => response.json())
                .then(icons => {
                    iconList.innerHTML = '';
                    icons.forEach(icon => {
                        const li = document.createElement('li');
                        li.className = 'list-group-item';
                        // On crée une image et un span pour le texte
                        li.innerHTML = `<img src="${icon.data}" alt="${icon.name}"> <span>${icon.name}</span>`;
                        iconList.appendChild(li);
                    });
                });
        });

        // --- Logique de la Sauvegarde avec Modal Bootstrap ---
        function saveConfig() {
            const data = {};
            for (let l = 0; l < NUM_LAYERS; l++) {
                for (let k = 1; k <= NUM_KEYS; k++) {
                    const keyId = `L${l}K${k}`;
                    const cmd = document.getElementById(`${keyId}_cmd`).value;
                    const label = document.getElementById(`${keyId}_label`).value;
                    const icon = document.getElementById(`${keyId}_icon`).value;
                    if (cmd) { data[keyId] = { cmd, label, icon }; }
                }
            }

            const statusModalEl = document.getElementById('saveStatusModal');
            const statusModal = new bootstrap.Modal(statusModalEl);
            const statusMessageEl = document.getElementById('saveStatusMessage');

            fetch('/save', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(data)
            })
            .then(response => {
                if (response.ok) {
                    statusMessageEl.textContent = 'Configuration sauvegardée !';
                    statusMessageEl.className = 'text-success'; // Couleur verte Bootstrap
                } else {
                    statusMessageEl.textContent = 'Erreur de sauvegarde.';
                    statusMessageEl.className = 'text-danger'; // Couleur rouge Bootstrap
                }
                statusModal.show();
                setTimeout(() => {
                    statusModal.hide();
                }, 2000); // Cache la modale après 2 secondes
            });
        }
        
        // --- Horloge ---
        const datetimeDiv = document.getElementById('datetime');
        setInterval(() => {
            const now = new Date();
            datetimeDiv.textContent = `${now.toLocaleDateString('fr-FR')} - ${now.toLocaleTimeString('fr-FR')}`;
        }, 1000);
    </script>
</body>
</html>
)rawliteral";

/* ------------------------------ Fin du code -------------------------------- */