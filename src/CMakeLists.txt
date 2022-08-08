# Add the threadpool subdirectory.
add_subdirectory(
    threadpool
)
list(APPEND EXTRA_LIBS threadpool)

# Add main.cpp and create the executable bin_main.
add_executable(
    bin_main
    main.cpp
)

# Link the EXTRA_LIBS to bin_main
target_link_libraries(bin_main PUBLIC
    ${EXTRA_LIBS}
)
