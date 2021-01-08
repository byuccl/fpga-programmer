add_library(sounds 
bcfire01_48k.wav.c
bcfire01.wav.c
gameBoyStartup.wav.c
gameOver48k.wav.c
gunEmpty48k.wav.c
ouch48k.wav.c
pacman_beginning_48k.wav.c#
pacmanDeath.wav.c
powerUp48k.wav.c
powerUp48k.wav.c
screamAndDie48k.wav.c
)

target_link_libraries(sounds ${330_LIBS})

