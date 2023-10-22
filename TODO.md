# TODO
## Vital
- [ ] 2023-10-22 - Subresouce for Mipmap Levels.
- [ ] 2023-10-22 - Dynamic Uniform Buffers.
- [ ] 2023-10-18 - CMake Asset Directory copyifdifferent (target AlbedoPrebuilder).
- [ ] 2023-09-5  - Implement ZReverse.

## Optimization
- [ ] 2023-09-12 - Delete GRIPushPreframeTask(), instead, using a Global CommandBuffer in Transfer Queue.
- [ ] 2023-09-2  - GRI manage Global Shaders.
- [ ] 2023-09-1  - Move commandbuffers to each Render Pass?

## Trival
- [ ] 2023-7-30 - Solve VMA Implement macro issue (now two RHI function difinitions in GRI.cc)

## DONE
- [X] 2023-09-4  - Add a hashmap to ConvertLayout()
- [X] 2023-09-3  - Modify Pipeline Cache path to APP asset directory.
- [X] 2023-09-2  - Render to Image(Render Target) and simplify the creation of Framebuffer in Render Pass. And use Blit in surface pass to blit image to Swapchain for presentation
- [X] 2023-09-2  - Fix Descriptor Set Layout duplication issue in pipeline.