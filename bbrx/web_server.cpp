#include "web_server.h"

BBRXWebServer::BBRXWebServer(int port) : server(port), is_running(false) {}

void BBRXWebServer::begin() {
    // Initialize LittleFS if not already done
    if (!LittleFS.begin(CONFIG_LFS_FORMAT_IF_FAIL, CONFIG_LFS_BASE_PATH, CONFIG_LFS_MAX_OPEN_FILES, CONFIG_LFS_PARTITION_LABEL)) {
        loge(LOG_TAG, "Failed to mount LittleFS");
        return;
    }

    // Setup routes
    server.on("/", HTTP_GET, [this]() { handleRoot(); });
    server.on("/upload", HTTP_POST, [this]() { handleUpload(); }, [this]() {
        HTTPUpload& upload = server.upload();
        if (upload.status == UPLOAD_FILE_START) {
            String filename = "/" + upload.filename;
            if (!filename.startsWith("/")) filename = "/" + filename;
            
            File file = LittleFS.open(filename, "w");
            if (!file) {
                loge(LOG_TAG, "Failed to open file for writing: %s", filename.c_str());
                return;
            }
            file.close();
        } else if (upload.status == UPLOAD_FILE_WRITE) {
            File file = LittleFS.open("/" + upload.filename, "a");
            if (file) {
                file.write(upload.buf, upload.currentSize);
                file.close();
            }
        } else if (upload.status == UPLOAD_FILE_END) {
            logi(LOG_TAG, "Upload complete: %s (%d bytes)", upload.filename.c_str(), upload.totalSize);
        }
    });
    
    server.on("/download", HTTP_GET, [this]() { handleDownload(); });
    server.on("/edit", HTTP_GET, [this]() { handleEdit(); });
    server.on("/save", HTTP_POST, [this]() { handleSave(); });
    server.on("/delete", HTTP_DELETE, [this]() { handleDelete(); });
    server.on("/list", HTTP_GET, [this]() { handleList(); });
    server.onNotFound([this]() { handleNotFound(); });

    server.begin();
    is_running = true;
    logi(LOG_TAG, "Web server started");
}

void BBRXWebServer::handle() {
    if (is_running) {
        server.handleClient();
    }
}

void BBRXWebServer::stop() {
    server.stop();
    is_running = false;
    logi(LOG_TAG, "Web server stopped");
}

void BBRXWebServer::handleRoot() {
    String html = F("<!DOCTYPE html>"
"<html>"
"<head>"
"    <meta charset=\"UTF-8\">"
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
"    <title>BBRX Config Manager</title>"
"    <style>"
"        * { margin: 0; padding: 0; box-sizing: border-box; }"
"        body { "
"            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;"
"            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);"
"            min-height: 100vh;"
"            padding: 20px;"
"        }"
"        .container {"
"            max-width: 900px;"
"            margin: 0 auto;"
"            background: white;"
"            border-radius: 16px;"
"            box-shadow: 0 20px 60px rgba(0,0,0,0.3);"
"            overflow: hidden;"
"        }"
"        .header {"
"            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);"
"            color: white;"
"            padding: 30px;"
"            text-align: center;"
"        }"
"        .header h1 { font-size: 2em; margin-bottom: 10px; }"
"        .header p { opacity: 0.9; }"
"        .content { padding: 30px; }"
"        .section {"
"            margin-bottom: 30px;"
"            padding: 20px;"
"            background: #f8f9fa;"
"            border-radius: 12px;"
"            border-left: 4px solid #667eea;"
"        }"
"        .section h2 {"
"            color: #667eea;"
"            margin-bottom: 15px;"
"            font-size: 1.3em;"
"        }"
"        .form-group { margin-bottom: 15px; }"
"        label {"
"            display: block;"
"            margin-bottom: 5px;"
"            color: #495057;"
"            font-weight: 500;"
"        }"
"        input[type=\"file\"], select {"
"            width: 100%;"
"            padding: 12px;"
"            border: 2px solid #dee2e6;"
"            border-radius: 8px;"
"            font-size: 14px;"
"            transition: border-color 0.3s;"
"        }"
"        input[type=\"file\"]:focus, select:focus {"
"            outline: none;"
"            border-color: #667eea;"
"        }"
"        textarea {"
"            width: 100%;"
"            min-height: 400px;"
"            padding: 15px;"
"            border: 2px solid #dee2e6;"
"            border-radius: 8px;"
"            font-family: 'Courier New', monospace;"
"            font-size: 13px;"
"            resize: vertical;"
"            transition: border-color 0.3s;"
"        }"
"        textarea:focus {"
"            outline: none;"
"            border-color: #667eea;"
"        }"
"        button {"
"            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);"
"            color: white;"
"            border: none;"
"            padding: 12px 24px;"
"            border-radius: 8px;"
"            cursor: pointer;"
"            font-size: 14px;"
"            font-weight: 600;"
"            transition: transform 0.2s, box-shadow 0.2s;"
"            margin-right: 10px;"
"        }"
"        button:hover {"
"            transform: translateY(-2px);"
"            box-shadow: 0 4px 12px rgba(102, 126, 234, 0.4);"
"        }"
"        button:active { transform: translateY(0); }"
"        .btn-danger {"
"            background: linear-gradient(135deg, #f093fb 0%, #f5576c 100%);"
"        }"
"        .btn-success {"
"            background: linear-gradient(135deg, #4facfe 0%, #00f2fe 100%);"
"        }"
"        .file-list {"
"            list-style: none;"
"            margin-top: 15px;"
"        }"
"        .file-item {"
"            background: white;"
"            padding: 12px 15px;"
"            margin-bottom: 8px;"
"            border-radius: 8px;"
"            display: flex;"
"            justify-content: space-between;"
"            align-items: center;"
"            transition: transform 0.2s;"
"        }"
"        .file-item:hover {"
"            transform: translateX(5px);"
"            box-shadow: 0 2px 8px rgba(0,0,0,0.1);"
"        }"
"        .status {"
"            padding: 12px;"
"            border-radius: 8px;"
"            margin-top: 15px;"
"            display: none;"
"        }"
"        .status.success {"
"            background: #d4edda;"
"            color: #155724;"
"            border: 1px solid #c3e6cb;"
"        }"
"        .status.error {"
"            background: #f8d7da;"
"            color: #721c24;"
"            border: 1px solid #f5c6cb;"
"        }"
"        .loading {"
"            display: none;"
"            text-align: center;"
"            padding: 20px;"
"        }"
"        .spinner {"
"            border: 3px solid #f3f3f3;"
"            border-top: 3px solid #667eea;"
"            border-radius: 50%;"
"            width: 40px;"
"            height: 40px;"
"            animation: spin 1s linear infinite;"
"            margin: 0 auto;"
"        }"
"        @keyframes spin {"
"            0% { transform: rotate(0deg); }"
"            100% { transform: rotate(360deg); }"
"        }"
"    </style>"
"</head>"
"<body>"
"    <div class=\"container\">"
"        <div class=\"header\">"
"            <h1>BBRX Config Manager</h1>"
"            <p>Manage your YAML configuration files</p>"
"        </div>"
"        "
"        <div class=\"content\">"
"            <div class=\"section\">"
"                <h2>Upload Config File</h2>"
"                <div class=\"form-group\">"
"                    <label for=\"uploadFile\">Choose a YAML file to upload:</label>"
"                    <input type=\"file\" id=\"uploadFile\" accept=\".yml,.yaml\">"
"                </div>"
"                <button onclick=\"uploadFile()\">Upload</button>"
"                <div id=\"uploadStatus\" class=\"status\"></div>"
"            </div>"
""
"            <div class=\"section\">"
"                <h2>Available Config Files</h2>"
"                <button onclick=\"refreshFileList()\">Refresh List</button>"
"                <ul id=\"fileList\" class=\"file-list\"></ul>"
"                <div class=\"loading\" id=\"fileListLoading\">"
"                    <div class=\"spinner\"></div>"
"                    <p>Loading files...</p>"
"                </div>"
"            </div>"
""
"            <div class=\"section\">"
"                <h2>Edit Config</h2>"
"                <div class=\"form-group\">"
"                    <label for=\"editFile\">Select file to edit:</label>"
"                    <select id=\"editFile\" onchange=\"loadFileForEdit()\">"
"                        <option value=\"\">-- Select a file --</option>"
"                    </select>"
"                </div>"
"                <div class=\"form-group\">"
"                    <textarea id=\"editContent\" placeholder=\"File content will appear here...\"></textarea>"
"                </div>"
"                <button class=\"btn-success\" onclick=\"saveFile()\">Save Changes</button>"
"                <button onclick=\"clearEditor()\">Clear</button>"
"                <div id=\"editStatus\" class=\"status\"></div>"
"            </div>"
"        </div>"
"    </div>"
""
"    <script>"
"        function showStatus(elementId, message, isError) {"
"            var status = document.getElementById(elementId);"
"            status.textContent = message;"
"            status.className = 'status ' + (isError ? 'error' : 'success');"
"            status.style.display = 'block';"
"            setTimeout(function() { status.style.display = 'none'; }, 5000);"
"        }"
""
"        function uploadFile() {"
"            var fileInput = document.getElementById('uploadFile');"
"            var file = fileInput.files[0];"
"            "
"            if (!file) {"
"                showStatus('uploadStatus', 'Please select a file first', true);"
"                return;"
"            }"
""
"            var formData = new FormData();"
"            formData.append('file', file);"
""
"            fetch('/upload', {"
"                method: 'POST',"
"                body: formData"
"            })"
"            .then(function(response) { return response.text(); })"
"            .then(function(data) {"
"                showStatus('uploadStatus', 'File uploaded successfully!');"
"                fileInput.value = '';"
"                refreshFileList();"
"            })"
"            .catch(function(error) {"
"                showStatus('uploadStatus', 'Upload failed: ' + error, true);"
"            });"
"        }"
""
"        function refreshFileList() {"
"            var fileList = document.getElementById('fileList');"
"            var loading = document.getElementById('fileListLoading');"
"            var editSelect = document.getElementById('editFile');"
"            "
"            fileList.innerHTML = '';"
"            loading.style.display = 'block';"
""
"            fetch('/list')"
"                .then(function(response) { return response.json(); })"
"                .then(function(files) {"
"                    loading.style.display = 'none';"
"                    "
"                    editSelect.innerHTML = '<option value=\"\">-- Select a file --</option>';"
"                    "
"                    for(var i = 0; i < files.length; i++) {"
"                        var file = files[i];"
"                        var li = document.createElement('li');"
"                        li.className = 'file-item';"
"                        li.innerHTML = '<span>' + file + '</span><div><button onclick=\"downloadFile(\\'' + file + '\\')\" >Download</button><button class=\"btn-danger\" onclick=\"deleteFile(\\'' + file + '\\')\">Delete</button></div>';"
"                        fileList.appendChild(li);"
"                        "
"                        var option = document.createElement('option');"
"                        option.value = file;"
"                        option.textContent = file;"
"                        editSelect.appendChild(option);"
"                    }"
""
"                    if (files.length === 0) {"
"                        fileList.innerHTML = '<li class=\"file-item\">No files found</li>';"
"                    }"
"                })"
"                .catch(function(error) {"
"                    loading.style.display = 'none';"
"                    showStatus('editStatus', 'Failed to load file list: ' + error, true);"
"                });"
"        }"
""
"        function downloadFile(filename) {"
"            window.location.href = '/download?file=' + encodeURIComponent(filename);"
"        }"
""
"        function deleteFile(filename) {"
"            if (!confirm('Are you sure you want to delete ' + filename + '?')) {"
"                return;"
"            }"
""
"            fetch('/delete?file=' + encodeURIComponent(filename), {"
"                method: 'DELETE'"
"            })"
"            .then(function(response) { return response.text(); })"
"            .then(function(data) {"
"                showStatus('editStatus', 'File deleted successfully!');"
"                refreshFileList();"
"            })"
"            .catch(function(error) {"
"                showStatus('editStatus', 'Delete failed: ' + error, true);"
"            });"
"        }"
""
"        function loadFileForEdit() {"
"            var filename = document.getElementById('editFile').value;"
"            if (!filename) return;"
""
"            fetch('/edit?file=' + encodeURIComponent(filename))"
"                .then(function(response) { return response.text(); })"
"                .then(function(content) {"
"                    document.getElementById('editContent').value = content;"
"                })"
"                .catch(function(error) {"
"                    showStatus('editStatus', 'Failed to load file: ' + error, true);"
"                });"
"        }"
""
"        function saveFile() {"
"            var filename = document.getElementById('editFile').value;"
"            var content = document.getElementById('editContent').value;"
""
"            if (!filename) {"
"                showStatus('editStatus', 'Please select a file to save', true);"
"                return;"
"            }"
""
"            fetch('/save?file=' + encodeURIComponent(filename), {"
"                method: 'POST',"
"                headers: { 'Content-Type': 'text/plain' },"
"                body: content"
"            })"
"            .then(function(response) { return response.text(); })"
"            .then(function(data) {"
"                showStatus('editStatus', 'File saved successfully!');"
"            })"
"            .catch(function(error) {"
"                showStatus('editStatus', 'Save failed: ' + error, true);"
"            });"
"        }"
""
"        function clearEditor() {"
"            document.getElementById('editFile').value = '';"
"            document.getElementById('editContent').value = '';"
"        }"
""
"        window.onload = refreshFileList;"
"    </script>"
"</body>"
"</html>");
    
    server.send(200, "text/html", html);
}

void BBRXWebServer::handleUpload() {
    server.send(200, "text/plain", "Upload complete");
}

void BBRXWebServer::handleDownload() {
    String filename = server.arg("file");
    if (filename == "") {
        server.send(400, "text/plain", "File parameter missing");
        return;
    }

    if (!filename.startsWith("/")) {
        filename = "/" + filename;
    }

    File file = LittleFS.open(filename, "r");
    if (!file) {
        server.send(404, "text/plain", "File not found");
        return;
    }

    server.streamFile(file, getContentType(filename));
    file.close();
}

void BBRXWebServer::handleEdit() {
    String filename = server.arg("file");
    if (filename == "") {
        server.send(400, "text/plain", "File parameter missing");
        return;
    }

    if (!filename.startsWith("/")) {
        filename = "/" + filename;
    }

    File file = LittleFS.open(filename, "r");
    if (!file) {
        server.send(404, "text/plain", "File not found");
        return;
    }

    String content = file.readString();
    file.close();
    
    server.send(200, "text/plain", content);
}

void BBRXWebServer::handleSave() {
    String filename = server.arg("file");
    if (filename == "") {
        server.send(400, "text/plain", "File parameter missing");
        return;
    }

    if (!filename.startsWith("/")) {
        filename = "/" + filename;
    }

    File file = LittleFS.open(filename, "w");
    if (!file) {
        server.send(500, "text/plain", "Failed to open file for writing");
        return;
    }

    file.print(server.arg("plain"));
    file.close();
    
    server.send(200, "text/plain", "File saved successfully");
    
    // Reload config if it's the active config file
    if (filename == CONFIG_FILE_PATH) {
        logi(LOG_TAG, "Reloading config after save...");
        load_config();
    }
}

void BBRXWebServer::handleDelete() {
    String filename = server.arg("file");
    if (filename == "") {
        server.send(400, "text/plain", "File parameter missing");
        return;
    }

    if (!filename.startsWith("/")) {
        filename = "/" + filename;
    }

    if (LittleFS.remove(filename)) {
        server.send(200, "text/plain", "File deleted successfully");
    } else {
        server.send(500, "text/plain", "Failed to delete file");
    }
}

void BBRXWebServer::handleList() {
    File root = LittleFS.open("/");
    String fileList = "[";
    
    File file = root.openNextFile();
    bool first = true;
    while (file) {
        if (!file.isDirectory()) {
            if (!first) fileList += ",";
            fileList += "\"" + String(file.name()) + "\"";
            first = false;
        }
        file = root.openNextFile();
    }
    
    fileList += "]";
    server.send(200, "application/json", fileList);
}

void BBRXWebServer::handleNotFound() {
    server.send(404, "text/plain", "Not found");
}

String BBRXWebServer::getContentType(String filename) {
    if (filename.endsWith(".yml") || filename.endsWith(".yaml")) return "text/yaml";
    else if (filename.endsWith(".txt")) return "text/plain";
    else if (filename.endsWith(".json")) return "application/json";
    return "application/octet-stream";
}