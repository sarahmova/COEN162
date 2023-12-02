sarah movahedi
coen 162 -- angela musurlian
9 may 2023
Coding Project 1 - Instructions on how to start the proxy

    1. download middleman.cpp
    2. go to firefox and type in neverssl.com
            this page should load like normal
    3. go to the top right corner of the firefox browser and click the three lines
    4. click on the settings tab
    5. click the search bar on the top right that readsa "find in setting"
    6. type in "proxy"
    7. click on the settings button that appears
    8. switch the radio button from "Use system proxy settings" to "manual proxy configuration"
    9. now you can change the HTTP proxy line --> type in 127.0.0.1
    10. for the port number type in 8080
    11. check the "also use this proxy for HTTPS" button
    12. open terminal and type command : g++ -o middle middleman.cpp
            this will compile middleman
    13. type ./middle 8080 8080 
            this should begin the proxy
    14. now, go back to firefox and refresh the page
    