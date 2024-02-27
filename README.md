# QT-Pokedex-app

Informations and dependencies

    Compiled and tested with Desktop 5.12.11 MinGW 32-bit and Desktop 5.12.11 MinGW 64-bit.
    Qt Framework
    C++ Standard Library

<h3 align="left">Languages and Tools:</h3>
<p align="left"> <a href="https://www.w3schools.com/cpp/" target="_blank" rel="noreferrer"> <img src="https://raw.githubusercontent.com/devicons/devicon/master/icons/cplusplus/cplusplus-original.svg" alt="cplusplus" width="40" height="40"/> </a> <a href="https://www.qt.io/" target="_blank" rel="noreferrer"> <img src="https://upload.wikimedia.org/wikipedia/commons/0/0b/Qt_logo_2016.svg" alt="qt" width="40" height="40"/> </a> </p>




    
<h3 align="left"> Overview:</h3>

The Pokedex project is a Qt C++ application designed to provide a comprehensive interface for browsing and comparing Pokemon data. It parses data from a CSV file containing information about various Pokemon, including their types, stats and images and stores the information in a dedicated pokemon struct. 

The struct is then manipulated by the application to allow users to filter Pokemons by type and compare the stats of different Pokemon using the radar chart provided by the Comparison Tool in the second tab.

Downsized images for table data visualization are also cached in a custom QPixMap object to optimize speed and efficiency.


<h3 align="left"> ▸TAB1: Pokedex :</h3>

Upon launching the application, users are presented with a table displaying a list of Pokemon. Each row represents a different Pokemon, and columns display various details such as the Pokemon's ID, name, types, and stats.
Users can filter the displayed Pokemon by their types using checkboxes provided for each type. By selecting one or more checkboxes, users can narrow down the list to only show Pokemon of the selected types.
   
<img src="https://github.com/FNNN98/QT-Pokedex-app/blob/main/pokedex.png?raw=true" width="800">


<img src="https://github.com/FNNN98/QT-Pokedex-app/blob/main/pokedex2.png?raw=true" width="800">

<h3 align="left"> ▸TAB2: Pokedex Comparison Tool </h3>

Users can click on a Pokemon in the list to select it for comparison. They can then select another Pokemon from a second list for comparison. The selected Pokemon's stats will be displayed in a radar chart for easy comparison of their attributes.
The radar chart provides a visual representation of each selected Pokemon's stats, allowing users to quickly compare attributes such as speed, HP, attack, defense, special attack, and special defense. The interface provide an error prompt if the same pokemon is selected in both listView instances.
  
<img src="https://github.com/FNNN98/QT-Pokedex-app/blob/main/pokedex3.png?raw=true" width="800">


<h3 align="left">Connect with me:</h3>
<p align="left">
<a href="https://twitter.com/axaro11" target="blank"><img align="center" src="https://raw.githubusercontent.com/rahuldkjain/github-profile-readme-generator/master/src/images/icons/Social/twitter.svg" alt="axaro11" height="30" width="40" /></a>
</p>

