Note: - Pour executer les parties suivantes, il faut avoir les csv placés dans le dossier coordinates.
      - Il faut également un dossier mdl_trajectories qui contiendra les résultats de la compression.
      - Il faut également un dossier clustering_results qui contiendra les résultats de clustering.

Pour lancer la compression: executer le script script_compress_all.sh

Pour lancer le clustering: executer la commande suivante:
    ./bin/main clustering <method> <nb_clusters> <nb_iterations> <max_nb_game> <nb_threads> <angle_factor> <perpendicular_factor> <parallel_factor>

    <method> : keamns, kmedoid, propa
    note: nb_clusters = 0 pour propa (nombre de clusters déterminé automatiquement)